// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/NotifyCustomerInformation.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string NotifyCustomerInformationRequest::get_type() const {
    return "NotifyCustomerInformation";
}

void to_json(json& j, const NotifyCustomerInformationRequest& k) {
    // the required parts of the message
    j = json{
        {"data", k.data},
        {"seqNo", k.seqNo},
        {"generatedAt", k.generatedAt.to_rfc3339()},
        {"requestId", k.requestId},
    };
    // the optional parts of the message
    if (k.tbc) {
        j["tbc"] = k.tbc.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyCustomerInformationRequest& k) {
    // the required parts of the message
    k.data = j.at("data");
    k.seqNo = j.at("seqNo");
    k.generatedAt = ocpp::DateTime(std::string(j.at("generatedAt")));
    k.requestId = j.at("requestId");

    // the optional parts of the message
    if (j.contains("tbc")) {
        k.tbc.emplace(j.at("tbc"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyCustomerInformationRequest \p k to the given output
/// stream \p os \returns an output stream with the NotifyCustomerInformationRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyCustomerInformationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyCustomerInformationResponse::get_type() const {
    return "NotifyCustomerInformationResponse";
}

void to_json(json& j, const NotifyCustomerInformationResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyCustomerInformationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyCustomerInformationResponse \p k to the given output
/// stream \p os \returns an output stream with the NotifyCustomerInformationResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyCustomerInformationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
