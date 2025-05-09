// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/RequestStopTransaction.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string RequestStopTransactionRequest::get_type() const {
    return "RequestStopTransaction";
}

void to_json(json& j, const RequestStopTransactionRequest& k) {
    // the required parts of the message
    j = json{
        {"transactionId", k.transactionId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, RequestStopTransactionRequest& k) {
    // the required parts of the message
    k.transactionId = j.at("transactionId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given RequestStopTransactionRequest \p k to the given output stream
/// \p os \returns an output stream with the RequestStopTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const RequestStopTransactionRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string RequestStopTransactionResponse::get_type() const {
    return "RequestStopTransactionResponse";
}

void to_json(json& j, const RequestStopTransactionResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::request_start_stop_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, RequestStopTransactionResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_request_start_stop_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given RequestStopTransactionResponse \p k to the given output stream
/// \p os \returns an output stream with the RequestStopTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const RequestStopTransactionResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
