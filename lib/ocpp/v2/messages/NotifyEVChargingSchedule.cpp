// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/NotifyEVChargingSchedule.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string NotifyEVChargingScheduleRequest::get_type() const {
    return "NotifyEVChargingSchedule";
}

void to_json(json& j, const NotifyEVChargingScheduleRequest& k) {
    // the required parts of the message
    j = json{
        {"timeBase", k.timeBase.to_rfc3339()},
        {"chargingSchedule", k.chargingSchedule},
        {"evseId", k.evseId},
    };
    // the optional parts of the message
    if (k.selectedChargingScheduleId) {
        j["selectedChargingScheduleId"] = k.selectedChargingScheduleId.value();
    }
    if (k.powerToleranceAcceptance) {
        j["powerToleranceAcceptance"] = k.powerToleranceAcceptance.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyEVChargingScheduleRequest& k) {
    // the required parts of the message
    k.timeBase = ocpp::DateTime(std::string(j.at("timeBase")));
    k.chargingSchedule = j.at("chargingSchedule");
    k.evseId = j.at("evseId");

    // the optional parts of the message
    if (j.contains("selectedChargingScheduleId")) {
        k.selectedChargingScheduleId.emplace(j.at("selectedChargingScheduleId"));
    }
    if (j.contains("powerToleranceAcceptance")) {
        k.powerToleranceAcceptance.emplace(j.at("powerToleranceAcceptance"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyEVChargingScheduleRequest \p k to the given output stream
/// \p os \returns an output stream with the NotifyEVChargingScheduleRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyEVChargingScheduleRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyEVChargingScheduleResponse::get_type() const {
    return "NotifyEVChargingScheduleResponse";
}

void to_json(json& j, const NotifyEVChargingScheduleResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::generic_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyEVChargingScheduleResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_generic_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyEVChargingScheduleResponse \p k to the given output
/// stream \p os \returns an output stream with the NotifyEVChargingScheduleResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyEVChargingScheduleResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
