// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_SETVARIABLES_HPP
#define OCPP_V2_SETVARIABLES_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP SetVariables message
struct SetVariablesRequest : public ocpp::Message {
    std::vector<SetVariableData> setVariableData;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetVariables message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetVariablesRequest \p k to a given json object \p j
void to_json(json& j, const SetVariablesRequest& k);

/// \brief Conversion from a given json object \p j to a given SetVariablesRequest \p k
void from_json(const json& j, SetVariablesRequest& k);

/// \brief Writes the string representation of the given SetVariablesRequest \p k to the given output stream \p os
/// \returns an output stream with the SetVariablesRequest written to
std::ostream& operator<<(std::ostream& os, const SetVariablesRequest& k);

/// \brief Contains a OCPP SetVariablesResponse message
struct SetVariablesResponse : public ocpp::Message {
    std::vector<SetVariableResult> setVariableResult;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetVariablesResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetVariablesResponse \p k to a given json object \p j
void to_json(json& j, const SetVariablesResponse& k);

/// \brief Conversion from a given json object \p j to a given SetVariablesResponse \p k
void from_json(const json& j, SetVariablesResponse& k);

/// \brief Writes the string representation of the given SetVariablesResponse \p k to the given output stream \p os
/// \returns an output stream with the SetVariablesResponse written to
std::ostream& operator<<(std::ostream& os, const SetVariablesResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_SETVARIABLES_HPP
