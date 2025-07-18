// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <random>

#include <everest/logging.hpp>
#include <ocpp/common/websocket/websocket_base.hpp>
#include <websocketpp_utils/base64.hpp>
namespace ocpp {

WebsocketBase::WebsocketBase() :
    m_is_connected(false),
    connected_callback(nullptr),
    stopped_connecting_callback(nullptr),
    message_callback(nullptr),
    reconnect_timer(nullptr),
    connection_attempts(1),
    ping_cleared(true),
    ping_elapsed_s(0),
    pong_elapsed_s(0),
    reconnect_backoff_ms(0),
    shutting_down(false) {

    set_connection_options_base(connection_options);

    this->ping_timer = std::make_unique<Everest::SteadyTimer>();
    const auto auth_key = connection_options.authorization_key;
    if (auth_key.has_value() and auth_key.value().length() < 16) {
        EVLOG_warning << "AuthorizationKey with only " << auth_key.value().length()
                      << " characters has been configured";
    }
}

WebsocketBase::~WebsocketBase() {
    this->cancel_reconnect_timer();
}

void WebsocketBase::set_connection_options_base(const WebsocketConnectionOptions& connection_options) {
    this->connection_options = connection_options;
}

void WebsocketBase::register_connected_callback(const std::function<void(OcppProtocolVersion protocol)>& callback) {
    this->connected_callback = callback;
}

void WebsocketBase::register_disconnected_callback(const std::function<void()>& callback) {
    this->disconnected_callback = callback;
}

void WebsocketBase::register_stopped_connecting_callback(
    const std::function<void(const WebsocketCloseReason reason)>& callback) {
    this->stopped_connecting_callback = callback;
}

void WebsocketBase::register_message_callback(const std::function<void(const std::string& message)>& callback) {
    this->message_callback = callback;
}

void WebsocketBase::register_connection_failed_callback(const std::function<void(ConnectionFailedReason)>& callback) {
    this->connection_failed_callback = callback;
}

bool WebsocketBase::initialized() {
    if (this->connected_callback == nullptr) {
        EVLOG_error << "Not properly initialized: please register connected callback.";
        return false;
    }
    if (this->stopped_connecting_callback == nullptr) {
        EVLOG_error << "Not properly initialized: please closed_callback.";
        return false;
    }
    if (this->message_callback == nullptr) {
        EVLOG_error << "Not properly initialized: please register message callback.";
        return false;
    }

    return true;
}

void WebsocketBase::disconnect(const WebsocketCloseReason code) {
    if (!this->initialized()) {
        EVLOG_error << "Cannot disconnect a websocket that was not initialized";
        return;
    }

    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        if (code == WebsocketCloseReason::Normal) {
            this->shutting_down = true;
        }

        if (this->reconnect_timer) {
            this->reconnect_timer.get()->cancel();
        }
    }

    if (this->ping_timer) {
        this->ping_timer->stop();
    }

    EVLOG_info << "Disconnecting websocket...";
    this->close(code, "");
}

bool WebsocketBase::is_connected() {
    return this->m_is_connected;
}

std::optional<std::string> WebsocketBase::getAuthorizationHeader() {
    std::optional<std::string> auth_header = std::nullopt;
    const auto authorization_key = this->connection_options.authorization_key;
    if (authorization_key.has_value()) {
        EVLOG_debug << "AuthorizationKey present, encoding authentication header";
        std::string plain_auth_header =
            this->connection_options.csms_uri.get_chargepoint_id() + ":" + authorization_key.value();

        // TODO (ioan): replace with libevse-security usage
        auth_header.emplace(std::string("Basic ") + ocpp::base64_encode(plain_auth_header));

        EVLOG_debug << "Basic Auth header: " << auth_header.value();
    }

    return auth_header;
}

void WebsocketBase::log_on_fail(const std::error_code& ec, const boost::system::error_code& transport_ec,
                                const int http_status) {
    EVLOG_error << "Failed to connect to websocket server"
                << ", error_code: " << ec.value() << ", reason: " << ec.message()
                << ", HTTP response code: " << http_status << ", category: " << ec.category().name()
                << ", transport error code: " << transport_ec.value()
                << ", Transport error category: " << transport_ec.category().name();
}

long WebsocketBase::get_reconnect_interval() {

    // We need to add 1 to the repeat times since the first try is already connection_attempt 1
    if (this->connection_attempts > (this->connection_options.retry_backoff_repeat_times + 1)) {
        return this->reconnect_backoff_ms;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, this->connection_options.retry_backoff_random_range_s);

    int random_number = distr(gen);

    if (this->connection_attempts == 1) {
        this->reconnect_backoff_ms = (this->connection_options.retry_backoff_wait_minimum_s + random_number) * 1000;
        return this->reconnect_backoff_ms;
    }

    this->reconnect_backoff_ms = this->reconnect_backoff_ms * 2 + (random_number * 1000);
    return this->reconnect_backoff_ms;
}

void WebsocketBase::cancel_reconnect_timer() {
    std::lock_guard<std::mutex> lk(this->reconnect_mutex);
    if (this->reconnect_timer) {
        this->reconnect_timer.get()->cancel();
    }
    this->reconnect_timer = nullptr;
}

void WebsocketBase::set_websocket_ping_interval(int32_t ping_interval_s, int32_t pong_timeout_s) {
    static constexpr int32_t PING_TIMER_INTERVAL = 1;

    if (this->ping_timer) {
        this->ping_timer->stop();
    }

    if (ping_interval_s > 0) {
        EVLOG_debug << "Started a ping interval of: " << ping_interval_s
                    << " s with a pong timeout of: " << pong_timeout_s << " s";

        this->ping_timer->interval(
            [this]() {
                if (false == ping_cleared.load()) { // We have a ping to which we require a response
                    pong_elapsed_s += PING_TIMER_INTERVAL;

                    // We have waited more than we should for the pong response
                    if (pong_elapsed_s > this->connection_options.pong_timeout_s) {
                        on_pong_timeout("Pong not received from server!");

                        // Always clear ping value on a fail
                        ping_cleared.store(true);

                        // Reset the elapsed time
                        ping_elapsed_s = pong_elapsed_s = 0;

                        // Clear the ping timer
                        if (this->ping_timer) {
                            this->ping_timer->stop();
                        }
                    }
                } else { // We need to see if we have to send the ping
                    ping_elapsed_s += PING_TIMER_INTERVAL;

                    if (ping_elapsed_s > this->connection_options.ping_interval_s) {
                        // Set the ping flag before sending it out
                        ping_cleared.store(false);
                        this->ping();

                        // Reset the elapsed time
                        ping_elapsed_s = pong_elapsed_s = 0;
                    }
                }
            },
            // Tick each second to see if we reached a timeout
            std::chrono::seconds(PING_TIMER_INTERVAL));
    }

    this->connection_options.ping_interval_s = ping_interval_s;
    this->connection_options.pong_timeout_s = pong_timeout_s;
}

void WebsocketBase::set_authorization_key(const std::string& authorization_key) {
    this->connection_options.authorization_key = authorization_key;
}

void WebsocketBase::on_pong_timeout(std::string msg) {
    EVLOG_info << "Reconnecting because of a pong timeout after " << this->connection_options.pong_timeout_s << "s"
               << " and with reason: " << msg;
    this->close(WebsocketCloseReason::ServiceRestart, "Pong timeout"); // application code will handle the reconnect
}

} // namespace ocpp
