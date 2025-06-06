// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/NotifyChargingLimit.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string NotifyChargingLimitRequest::get_type() const {
    return "NotifyChargingLimit";
}

void to_json(json& j, const NotifyChargingLimitRequest& k) {
    // the required parts of the message
    j = json{
        {"chargingLimit", k.chargingLimit},
    };
    // the optional parts of the message
    if (k.chargingSchedule) {
        j["chargingSchedule"] = json::array();
        for (auto val : k.chargingSchedule.value()) {
            j["chargingSchedule"].push_back(val);
        }
    }
    if (k.evseId) {
        j["evseId"] = k.evseId.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyChargingLimitRequest& k) {
    // the required parts of the message
    k.chargingLimit = j.at("chargingLimit");

    // the optional parts of the message
    if (j.contains("chargingSchedule")) {
        json arr = j.at("chargingSchedule");
        std::vector<ChargingSchedule> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.chargingSchedule.emplace(vec);
    }
    if (j.contains("evseId")) {
        k.evseId.emplace(j.at("evseId"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyChargingLimitRequest \p k to the given output stream \p
/// os \returns an output stream with the NotifyChargingLimitRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyChargingLimitRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyChargingLimitResponse::get_type() const {
    return "NotifyChargingLimitResponse";
}

void to_json(json& j, const NotifyChargingLimitResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyChargingLimitResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyChargingLimitResponse \p k to the given output stream \p
/// os \returns an output stream with the NotifyChargingLimitResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyChargingLimitResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
