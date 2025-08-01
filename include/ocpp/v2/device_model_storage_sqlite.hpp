// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef DEVICE_MODEL_STORAGE_SQLITE_HPP
#define DEVICE_MODEL_STORAGE_SQLITE_HPP

#include <filesystem>
#include <sqlite3.h>

#include <everest/database/sqlite/connection.hpp>
#include <everest/logging.hpp>
#include <ocpp/v2/device_model_storage_interface.hpp>

namespace ocpp {
namespace v2 {

class DeviceModelStorageSqlite : public DeviceModelStorageInterface {

private:
    std::unique_ptr<everest::db::sqlite::ConnectionInterface> db;

    int get_component_id(const Component& component_id);

    int get_variable_id(const Component& component_id, const Variable& variable_id);

    void initialize_connection(const fs::path& db_path);

public:
    /// \brief Opens SQLite connection at given \p db_path
    ///
    /// If init_db is true, all other paths must be given as well.
    ///
    /// \param db_path              Path to database
    /// \param migration_files_path Path to the migration files to initialize the database (only needs to be set if
    ///                             `init_db` is true)
    /// \param config_path          Path to the device model config used to initialize the database
    ///
    explicit DeviceModelStorageSqlite(const fs::path& db_path, const std::filesystem::path& migration_files_path,
                                      const std::filesystem::path& config_path);

    /// \brief Opens SQLite connection at given \p db_path
    ///
    /// \param db_path              Path to database
    /// \param migration_files_path Path to the migration files to initialize the database
    ///
    DeviceModelStorageSqlite(const fs::path& db_path, const fs::path& migration_files_path);

    /// \brief Opens SQLite connection at given \p db_path
    ///
    /// \param db_path Path to database
    DeviceModelStorageSqlite(const fs::path& db_path);

    ~DeviceModelStorageSqlite() = default;

    std::map<Component, std::map<Variable, VariableMetaData>> get_device_model() final;

    std::optional<VariableAttribute> get_variable_attribute(const Component& component_id, const Variable& variable_id,
                                                            const AttributeEnum& attribute_enum) final;

    std::vector<VariableAttribute> get_variable_attributes(const Component& component_id, const Variable& variable_id,
                                                           const std::optional<AttributeEnum>& attribute_enum) final;

    SetVariableStatusEnum set_variable_attribute_value(const Component& component_id, const Variable& variable_id,
                                                       const AttributeEnum& attribute_enum, const std::string& value,
                                                       const std::string& source) final;

    std::optional<VariableMonitoringMeta> set_monitoring_data(const SetMonitoringData& data,
                                                              const VariableMonitorType type) final;

    bool update_monitoring_reference(const int32_t monitor_id, const std::string& reference_value) final;

    std::vector<VariableMonitoringMeta> get_monitoring_data(const std::vector<MonitoringCriterionEnum>& criteria,
                                                            const Component& component_id,
                                                            const Variable& variable_id) final;

    ClearMonitoringStatusEnum clear_variable_monitor(int monitor_id, bool allow_protected) final;

    int32_t clear_custom_variable_monitors() final;

    void check_integrity() final;
};

} // namespace v2
} // namespace ocpp

#endif // DEVICE_MODEL_STORAGE_SQLITE_HPP
