// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// execute: ./libocpp_unit_tests --gtest_filter=ProfileTestsA.*

#include <chrono>

#include "ocpp/common/types.hpp"
#include "ocpp/v16/ocpp_enums.hpp"
#include "ocpp/v16/profile.hpp"
#include "profile_tests_common.hpp"
#include <gtest/gtest.h>
#include <optional>

// ----------------------------------------------------------------------------
// Test anonymous namespace
namespace {
using namespace ocpp::v16;
using namespace ocpp;
using ocpp::v16::period_entry_t;
using std::chrono::minutes;
using std::chrono::seconds;

constexpr CompositeScheduleDefaultLimits DEFAULT_LIMITS = {48, 33120, 3};
constexpr int default_supply_voltage = 230;

// ----------------------------------------------------------------------------
// Test charging profiles

const DateTime profileAbsolute_validFrom("2024-01-01T12:00:00Z");
const DateTime profileAbsolute_startSchedule("2024-01-01T12:02:00Z");
const DateTime profileAbsolute_validTo("2024-01-01T14:00:00Z");
const ChargingProfile profileAbsolute{
    301,                                          // chargingProfileId
    5,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                32.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                1800,         // startPeriod
                31.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                2700,         // startPeriod
                30.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        3600,                          // optional - int32_t duration
        profileAbsolute_startSchedule, // optional - ocpp::DateTime - startSchedule
        std::nullopt,                  // optional - float - minChargingRate
    },                                 // chargingSchedule
    std::nullopt,                      // transactionId
    std::nullopt,                      // recurrencyKind
    profileAbsolute_validFrom,         // validFrom
    profileAbsolute_validTo,           // validTo
};

const DateTime profileRelative_validFrom("2024-01-01T12:00:00Z");
const DateTime profileRelative_validTo("2024-01-01T14:00:00Z");
const ChargingProfile profileRelative{
    301,                                          // chargingProfileId
    5,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Relative,            // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                32.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                1800,         // startPeriod
                31.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                2700,         // startPeriod
                30.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        3600,                  // optional - int32_t duration
        std::nullopt,          // optional - ocpp::DateTime - startSchedule
        std::nullopt,          // optional - float - minChargingRate
    },                         // chargingSchedule
    std::nullopt,              // transactionId
    std::nullopt,              // recurrencyKind
    profileRelative_validFrom, // validFrom
    profileRelative_validTo,   // validTo
};

// 2024-01-01 is a Monday, daily starting at 08:00
const DateTime profileRecurring_validFrom("2024-01-01T12:00:00Z");
const DateTime profileRecurring_startSchedule("2024-01-01T08:00:00Z");
const DateTime profileRecurring_validTo("2024-02-01T12:00:00Z");
const ChargingProfile profileRecurring{
    301,                                          // chargingProfileId
    5,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Recurring,           // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                32.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                1800,         // startPeriod
                31.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                2700,         // startPeriod
                30.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        3600,                           // optional - int32_t duration
        profileRecurring_startSchedule, // optional - ocpp::DateTime - startSchedule
        std::nullopt,                   // optional - float - minChargingRate
    },                                  // chargingSchedule
    std::nullopt,                       // transactionId
    RecurrencyKindType::Daily,          // recurrencyKind
    profileRecurring_validFrom,         // validFrom
    profileRecurring_validTo,           // validTo
};

// ----------------------------------------------------------------------------
// Test cases - calculate_start()

TEST(ProfileTestsA, calculateStartAbsolute) {
    // profile not started yet
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T11:50:00Z");
    auto start_time = calculate_start(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], profileAbsolute_startSchedule);

    // profile started
    now = DateTime("2024-01-01T12:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], profileAbsolute_startSchedule);

    // profile finished
    now = DateTime("2024-01-01T14:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], profileAbsolute_startSchedule);

    // session started
    auto session_start = DateTime("2024-01-01T12:05:00Z");
    now = DateTime("2024-01-01T12:10:00Z");
    start_time = calculate_start(now, end, session_start, profileAbsolute);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], profileAbsolute_startSchedule);
}

TEST(ProfileTestsA, calculateStartRelative) {
    // profile not started yet
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T11:50:00Z");
    auto start_time = calculate_start(now, end, std::nullopt, profileRelative);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], now);

    // profile started
    now = DateTime("2024-01-01T12:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profileRelative);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], now);

    // profile finished
    now = DateTime("2024-01-01T14:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profileRelative);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], now);

    // session started before profile
    auto session_start = DateTime("2024-01-01T11:50:00Z");
    now = DateTime("2024-01-01T12:10:00Z");
    start_time = calculate_start(now, end, session_start, profileRelative);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], session_start);

    // session started during profile
    session_start = DateTime("2024-01-01T12:50:00Z");
    now = DateTime("2024-01-01T12:55:00Z");
    start_time = calculate_start(now, end, session_start, profileRelative);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], session_start);

    // session started after profile
    session_start = DateTime("2024-01-01T14:10:00Z");
    now = DateTime("2024-01-01T14:15:00Z");
    start_time = calculate_start(now, end, session_start, profileRelative);
    ASSERT_EQ(start_time.size(), 1);
    EXPECT_EQ(start_time[0], session_start);
}

TEST(ProfileTestsA, calculateStartReccuringDaily) {
    // Daily at 08:00
    // profile not started yet
    DateTime now("2024-01-01T11:50:00Z");
    DateTime end("2024-01-02T20:50:00Z");
    auto start_time = calculate_start(now, end, std::nullopt, profileRecurring);
    // start time is before profile is valid
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0].to_rfc3339(), "2024-01-01T08:00:00.000Z");
    EXPECT_EQ(start_time[1].to_rfc3339(), "2024-01-02T08:00:00.000Z");

    // profile started
    now = DateTime("2024-01-01T12:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profileRecurring);
    // start time is before profile is valid
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-01T08:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-02T08:00:00.000Z");

    now = DateTime("2024-01-02T07:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profileRecurring);
    // start time is before profile is valid (and the previous day)
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-01T08:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-02T08:00:00.000Z");

    now = DateTime("2024-01-02T08:10:00Z");
    end = DateTime("2024-01-03T20:50:00Z");
    start_time = calculate_start(now, end, std::nullopt, profileRecurring);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-02T08:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-03T08:00:00.000Z");

    now = DateTime("2024-01-02T23:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profileRecurring);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-02T08:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-03T08:00:00.000Z");

    now = DateTime("2024-01-03T07:10:00Z");
    ASSERT_EQ(start_time.size(), 2);
    start_time = calculate_start(now, end, std::nullopt, profileRecurring);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-02T08:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-03T08:00:00.000Z");

    // profile finished
    now = DateTime("2024-02-03T14:10:00Z");
    end = DateTime("2024-02-04T20:50:00Z");
    start_time = calculate_start(now, end, std::nullopt, profileRecurring);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-02-03T08:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-02-04T08:00:00.000Z");

    // session started
    auto session_start = DateTime("2024-01-05T11:50:00Z");
    now = DateTime("2024-01-05T12:10:00");
    end = DateTime("2024-01-06T20:50:00Z");
    start_time = calculate_start(now, end, session_start, profileRecurring);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-05T08:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-06T08:00:00.000Z");
}

TEST(ProfileTestsA, calculateStartReccuringWeekly) {
    // Wednesdays at 16:00
    auto profile{profileRecurring};
    profile.recurrencyKind = RecurrencyKindType::Weekly;
    profile.chargingSchedule.startSchedule = DateTime("2024-01-03T16:00:00Z");

    // profile not started yet
    DateTime now("2024-01-01T11:50:00Z");
    DateTime end("2024-01-07T20:50:00Z");
    auto start_time = calculate_start(now, end, std::nullopt, profile);
    // start time is before profile is valid
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2023-12-27T16:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-03T16:00:00.000Z");

    // profile started
    now = DateTime("2024-01-01T12:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profile);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2023-12-27T16:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-03T16:00:00.000Z");

    now = DateTime("2024-01-03T07:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profile);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2023-12-27T16:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-03T16:00:00.000Z");

    now = DateTime("2024-01-03T23:10:00Z");
    end = DateTime("2024-01-10T20:50:00Z");
    start_time = calculate_start(now, end, std::nullopt, profile);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-03T16:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-10T16:00:00.000Z");

    now = DateTime("2024-01-04T23:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profile);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-03T16:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-10T16:00:00.000Z");

    now = DateTime("2024-01-10T07:10:00Z");
    start_time = calculate_start(now, end, std::nullopt, profile);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-03T16:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-10T16:00:00.000Z");

    now = DateTime("2024-01-10T20:10:00Z");
    end = DateTime("2024-01-17T20:50:00Z");
    start_time = calculate_start(now, end, std::nullopt, profile);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-10T16:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-17T16:00:00.000Z");

    // profile finished
    now = DateTime("2024-02-03T14:10:00Z");
    end = DateTime("2024-02-10T20:50:00Z");
    start_time = calculate_start(now, end, std::nullopt, profile);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-31T16:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-02-07T16:00:00.000Z");

    // session started
    auto session_start = DateTime("2024-01-05T11:50:00Z");
    now = DateTime("2024-01-04T23:10:00Z");
    end = DateTime("2024-01-12T20:50:00Z");
    start_time = calculate_start(now, end, session_start, profile);
    ASSERT_EQ(start_time.size(), 2);
    EXPECT_EQ(start_time[0], "2024-01-03T16:00:00.000Z");
    EXPECT_EQ(start_time[1], "2024-01-10T16:00:00.000Z");
}

// ----------------------------------------------------------------------------
// Test cases - calculate_profile_entry()

TEST(ProfileTestsA, calculateProfileEntryAbsolute0) {
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileAbsolute, 0);
    ASSERT_EQ(res.size(), 1);

    const auto& entry = res[0];

    EXPECT_EQ(entry.start, "2024-01-01T12:02:00Z");
    EXPECT_EQ(entry.end, "2024-01-01T12:32:00Z");
    EXPECT_EQ(entry.limit, profileAbsolute.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry.number_phases);
    EXPECT_EQ(entry.stack_level, profileAbsolute.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryAbsolute1) {
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileAbsolute, 1);
    ASSERT_EQ(res.size(), 1);

    const auto& entry = res[0];

    EXPECT_EQ(entry.start, "2024-01-01T12:32:00Z");
    EXPECT_EQ(entry.end, "2024-01-01T12:47:00Z");
    EXPECT_EQ(entry.limit, profileAbsolute.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry.number_phases);
    EXPECT_EQ(entry.stack_level, profileAbsolute.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryAbsolute2) {
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileAbsolute, 2);
    ASSERT_EQ(res.size(), 1);

    const auto& entry = res[0];

    EXPECT_EQ(entry.start, "2024-01-01T12:47:00Z");
    EXPECT_EQ(entry.end, "2024-01-01T13:02:00Z");
    EXPECT_EQ(entry.limit, profileAbsolute.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry.number_phases);
    EXPECT_EQ(entry.stack_level, profileAbsolute.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryAbsolute3) {
    // index out of range
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileAbsolute, 3);
    ASSERT_EQ(res.size(), 0);
}

TEST(ProfileTestsA, calculateProfileEntryAbsoluteNoDuration) {
    auto profile{profileAbsolute};
    profile.chargingSchedule.duration = std::nullopt;

    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 2);
    ASSERT_EQ(res.size(), 1);

    const auto& entry = res[0];

    EXPECT_EQ(entry.start, "2024-01-01T12:47:00Z");
    EXPECT_EQ(entry.end, "2024-01-01T14:00:00Z");
    EXPECT_EQ(entry.limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry.number_phases);
    EXPECT_EQ(entry.stack_level, profile.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryAbsoluteExpired) {
    auto profile{profileAbsolute};
    profile.chargingSchedule.duration = std::nullopt;

    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T18:00:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 1);
    ASSERT_EQ(res.size(), 0);
}

TEST(ProfileTestsA, calculateProfileEntryRelative0) {
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:20:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileRelative, 0);
    ASSERT_EQ(res.size(), 1);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T12:20:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:50:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    const auto session_start = DateTime("2024-01-01T12:15:00Z");
    res = calculate_profile_entry(now, end, session_start, profileRelative, 0);
    ASSERT_EQ(res.size(), 1);

    entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T12:15:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:45:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRelative1) {
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:20:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileRelative, 1);
    ASSERT_EQ(res.size(), 1);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T12:50:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:05:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    const auto session_start = DateTime("2024-01-01T12:15:00Z");
    res = calculate_profile_entry(now, end, session_start, profileRelative, 1);
    ASSERT_EQ(res.size(), 1);

    entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T12:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:00:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRelative2) {
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:20:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileRelative, 2);
    ASSERT_EQ(res.size(), 1);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T13:05:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:20:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    const auto session_start = DateTime("2024-01-01T12:15:00Z");
    res = calculate_profile_entry(now, end, session_start, profileRelative, 2);
    ASSERT_EQ(res.size(), 1);

    entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T13:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:15:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRelative3) {
    // index out of range
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:20:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileRelative, 3);
    ASSERT_EQ(res.size(), 0);

    const auto session_start = DateTime("2024-01-01T12:15:00Z");
    res = calculate_profile_entry(now, end, session_start, profileRelative, 3);
    ASSERT_EQ(res.size(), 0);
}

TEST(ProfileTestsA, calculateProfileEntryRelativeNoDuration) {
    auto profile{profileRelative};
    profile.chargingSchedule.duration = std::nullopt;

    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T12:20:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 2);
    ASSERT_EQ(res.size(), 1);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T13:05:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T14:00:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);

    const auto session_start = DateTime("2024-01-01T12:15:00Z");
    res = calculate_profile_entry(now, end, session_start, profile, 2);
    ASSERT_EQ(res.size(), 1);

    entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T13:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T14:00:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRelativeExpired) {
    auto profile{profileRelative};
    profile.chargingSchedule.duration = std::nullopt;

    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T18:00:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 1);
    ASSERT_EQ(res.size(), 0);

    const auto session_start = DateTime("2024-01-01T12:15:00Z");
    res = calculate_profile_entry(now, end, session_start, profile, 1);
    ASSERT_EQ(res.size(), 0);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringDaily0) {
    // Daily at 08:00
    DateTime end("2024-01-03T20:50:00Z");
    DateTime now("2024-01-02T08:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileRecurring, 0);
    ASSERT_EQ(res.size(), 2);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-02T08:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-02T08:30:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-03T08:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T08:30:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringDaily1) {
    DateTime end("2024-01-03T20:50:00Z");
    DateTime now("2024-01-02T08:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileRecurring, 1);
    ASSERT_EQ(res.size(), 2);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-02T08:30:00Z");
    EXPECT_EQ(entry->end, "2024-01-02T08:45:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-03T08:30:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T08:45:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringDaily2) {
    DateTime end("2024-01-03T20:50:00Z");
    DateTime now("2024-01-02T08:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileRecurring, 2);
    ASSERT_EQ(res.size(), 2);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-02T08:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-02T09:00:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-03T08:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T09:00:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringDaily3) {
    // index out of range
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T08:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileRecurring, 3);
    ASSERT_EQ(res.size(), 0);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringDailyNoDuration) {
    auto profile{profileRecurring};
    profile.chargingSchedule.duration = std::nullopt;

    DateTime end("2024-01-04T08:00:00Z");
    DateTime now("2024-01-02T08:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 2);
    ASSERT_GE(res.size(), 2);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-02T08:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T08:00:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-03T08:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-04T08:00:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringDailyExpired) {
    auto profile{profileRecurring};
    profile.chargingSchedule.duration = std::nullopt;

    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-03-01T08:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 1);
    ASSERT_EQ(res.size(), 0);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringDailyBeforeValid) {
    // Daily at 08:00 valid from 12:00 2024-01-01 duration 1 hour

    // both periods complete before the profile is valid
    DateTime now("2023-12-28T08:10:00Z");
    DateTime end("2023-12-30T20:50:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profileRecurring, 2);
    ASSERT_EQ(res.size(), 0);

    // only second period is valid
    now = DateTime("2024-01-01T08:10:00Z");
    end = DateTime("2024-01-02T20:50:00Z");
    res = calculate_profile_entry(now, end, std::nullopt, profileRecurring, 2);
    ASSERT_EQ(res.size(), 1);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-02T08:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-02T09:00:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);

    // change profile so that there is no duration (profile covers whole day)
    // start is when the profile becomes valid
    auto profile{profileRecurring};
    profile.chargingSchedule.duration = std::nullopt;

    end = DateTime("2024-01-03T20:50:00Z");
    res = calculate_profile_entry(now, end, std::nullopt, profile, 2);
    ASSERT_GE(res.size(), 2);

    entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T12:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-02T08:00:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-02T08:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T08:00:00Z");
    EXPECT_EQ(entry->limit, profileRecurring.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRecurring.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringWeekly0) {
    // Wednesdays from 16:00
    auto profile{profileRecurring};
    profile.recurrencyKind = RecurrencyKindType::Weekly;
    profile.chargingSchedule.startSchedule = DateTime("2024-01-03T16:00:00Z");

    DateTime now("2024-01-03T16:10:00Z");
    DateTime end("2024-01-10T20:50:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 0);
    ASSERT_EQ(res.size(), 2);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-03T16:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T16:30:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-10T16:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-10T16:30:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringWeekly1) {
    // Wednesdays from 16:00
    auto profile{profileRecurring};
    profile.recurrencyKind = RecurrencyKindType::Weekly;
    profile.chargingSchedule.startSchedule = DateTime("2024-01-03T16:00:00Z");

    DateTime now("2024-01-03T16:10:00Z");
    DateTime end("2024-01-10T20:50:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 1);
    ASSERT_EQ(res.size(), 2);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-03T16:30:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T16:45:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-10T16:30:00Z");
    EXPECT_EQ(entry->end, "2024-01-10T16:45:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringWeekly2) {
    // Wednesdays from 16:00
    auto profile{profileRecurring};
    profile.recurrencyKind = RecurrencyKindType::Weekly;
    profile.chargingSchedule.startSchedule = DateTime("2024-01-03T16:00:00Z");

    DateTime now("2024-01-03T16:10:00Z");
    DateTime end("2024-01-10T20:50:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 2);
    ASSERT_EQ(res.size(), 2);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-03T16:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T17:00:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-10T16:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-10T17:00:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringWeekly3) {
    // index out of range
    // Wednesdays from 16:00
    auto profile{profileRecurring};
    profile.recurrencyKind = RecurrencyKindType::Weekly;
    profile.chargingSchedule.startSchedule = DateTime("2024-01-03T16:00:00Z");

    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-03T16:10:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 3);
    ASSERT_EQ(res.size(), 0);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringWeeklyNoDuration) {
    // Wednesdays from 16:00
    auto profile{profileRecurring};
    profile.recurrencyKind = RecurrencyKindType::Weekly;
    profile.chargingSchedule.startSchedule = DateTime("2024-01-03T16:00:00Z");
    profile.chargingSchedule.duration = std::nullopt;

    DateTime now("2024-01-03T16:10:00Z");
    DateTime end("2024-01-17T20:50:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 2);
    ASSERT_GE(res.size(), 2);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-03T16:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-10T16:00:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-10T16:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-17T16:00:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringWeeklyExpired) {
    // Wednesdays from 16:00
    auto profile{profileRecurring};
    profile.recurrencyKind = RecurrencyKindType::Weekly;
    profile.chargingSchedule.startSchedule = DateTime("2024-01-03T16:00:00Z");
    profile.chargingSchedule.duration = std::nullopt;

    DateTime now("2024-03-01T08:10:00Z");
    DateTime end("2024-03-10T20:50:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 1);
    ASSERT_EQ(res.size(), 0);
}

TEST(ProfileTestsA, calculateProfileEntryRecurringWeeklyBeforeValid) {
    // Wednesdays from 16:00
    auto profile{profileRecurring};
    profile.recurrencyKind = RecurrencyKindType::Weekly;
    profile.chargingSchedule.startSchedule = DateTime("2024-01-03T16:00:00Z");

    // both periods complete before the profile is valid
    DateTime now("2023-12-27T08:10:00Z");
    DateTime end("2024-01-01T20:50:00Z");
    auto res = calculate_profile_entry(now, end, std::nullopt, profile, 2);
    ASSERT_EQ(res.size(), 0);

    // only second period is valid
    now = DateTime("2023-12-30T08:10:00Z");
    end = DateTime("2024-01-03T20:50:00Z");
    res = calculate_profile_entry(now, end, std::nullopt, profile, 2);
    ASSERT_EQ(res.size(), 1);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-03T16:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T17:00:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);

    // change profile so that there is no duration (profile covers whole week)
    // start is when the profile becomes valid
    profile.chargingSchedule.duration = std::nullopt;

    end = DateTime("2024-01-10T20:50:00Z");
    res = calculate_profile_entry(now, end, std::nullopt, profile, 2);
    ASSERT_GE(res.size(), 2);

    entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T12:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-03T16:00:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);

    entry = &res[1];

    EXPECT_EQ(entry->start, "2024-01-03T16:45:00Z");
    EXPECT_EQ(entry->end, "2024-01-10T16:00:00Z");
    EXPECT_EQ(entry->limit, profile.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profile.stackLevel);
}

// ----------------------------------------------------------------------------
// Test cases - calculate_profile()

TEST(ProfileTestsA, calculateProfileAbsolute) {
    // schedule starts at 12:02 for 1 hour

    // before start
    DateTime now("2024-01-01T08:10:00Z");
    DateTime end("2024-01-01T20:50:00Z");

    // expecting all periods to be included
    auto res = calculate_profile(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(res.size(), profileAbsolute.chargingSchedule.chargingSchedulePeriod.size());

    DateTime session_start("2023-12-27T08:05:00Z");
    auto res_session = calculate_profile(now, end, session_start, profileAbsolute);
    // std::cout << res << std::endl;
    ASSERT_EQ(res_session.size(), profileAbsolute.chargingSchedule.chargingSchedulePeriod.size());

    // the session start should not change the result
    EXPECT_EQ(res, res_session);
    EXPECT_TRUE(validate_profile_result(res));
    // std::cout << res;

    // just before start
    now = DateTime("2024-01-01T12:01:00Z");

    // expecting all periods to be included
    res = calculate_profile(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(res.size(), profileAbsolute.chargingSchedule.chargingSchedulePeriod.size());

    res_session = calculate_profile(now, end, session_start, profileAbsolute);
    ASSERT_EQ(res_session.size(), profileAbsolute.chargingSchedule.chargingSchedulePeriod.size());

    // the session start should not change the result
    EXPECT_EQ(res, res_session);
    EXPECT_TRUE(validate_profile_result(res));

    // during
    now = DateTime("2024-01-01T12:40:00Z");

    // expecting 2 periods to be included
    res = calculate_profile(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(res.size(), 2);

    res_session = calculate_profile(now, end, session_start, profileAbsolute);
    ASSERT_EQ(res_session.size(), 2);

    // the session start should not change the result
    EXPECT_EQ(res, res_session);
    EXPECT_TRUE(validate_profile_result(res));

    // after
    now = DateTime("2024-01-01T14:01:00Z");

    // expecting no periods to be included
    res = calculate_profile(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(res.size(), 0);

    res_session = calculate_profile(now, end, session_start, profileAbsolute);
    ASSERT_EQ(res_session.size(), 0);

    // the session start should not change the result
    EXPECT_EQ(res, res_session);
    EXPECT_TRUE(validate_profile_result(res));
}

TEST(ProfileTestsA, calculateProfileAbsoluteLimited) {
    // schedule starts at 12:02 for 1 hour

    // before start
    DateTime now("2024-01-01T08:10:00Z");
    DateTime end(now.to_time_point() + minutes(20));

    // expecting no periods
    auto res = calculate_profile(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(res.size(), 0);

    // just before start
    now = DateTime("2024-01-01T12:01:00Z");
    end = DateTime(now.to_time_point() + minutes(20));

    // expecting a single period
    res = calculate_profile(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(res.size(), 1);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T12:02:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:32:00Z");
    EXPECT_EQ(entry->limit, profileAbsolute.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileAbsolute.stackLevel);

    EXPECT_TRUE(validate_profile_result(res));

    // during
    now = DateTime("2024-01-01T12:40:00Z");
    end = DateTime(now.to_time_point() + minutes(20));

    // expecting 2 periods to be included
    res = calculate_profile(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(res.size(), 2);

    entry = &res[0];
    EXPECT_EQ(entry->start, "2024-01-01T12:32:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:47:00Z");
    EXPECT_EQ(entry->limit, profileAbsolute.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileAbsolute.stackLevel);

    entry = &res[1];
    EXPECT_EQ(entry->start, "2024-01-01T12:47:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:02:00Z");
    EXPECT_EQ(entry->limit, profileAbsolute.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileAbsolute.stackLevel);

    EXPECT_TRUE(validate_profile_result(res));

    // after
    now = DateTime("2024-01-01T14:01:00Z");
    end = DateTime(now.to_time_point() + minutes(20));

    // expecting no periods to be included
    res = calculate_profile(now, end, std::nullopt, profileAbsolute);
    ASSERT_EQ(res.size(), 0);
}

TEST(ProfileTestsA, calculateProfileRelative) {
    // valid 12:00 to 14:00 for 1 hour

    // before start
    DateTime end("2024-01-01T20:50:00Z");
    DateTime now("2024-01-01T08:10:00Z");

    // expecting no periods to be included
    auto res = calculate_profile(now, end, std::nullopt, profileRelative);
    ASSERT_EQ(res.size(), 0);

    DateTime session_start("2023-12-27T08:05:00Z");
    auto res_session = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res_session.size(), 0);

    // the session start should not change the result
    EXPECT_EQ(res, res_session);
    EXPECT_TRUE(validate_profile_result(res));
    // std::cout << res;
    // std::cout << res_session;

    // just before start
    now = DateTime("2024-01-01T11:58:00Z");
    session_start = DateTime("2024-01-01T11:55:00Z");

    // expecting all periods to be included
    res = calculate_profile(now, end, std::nullopt, profileRelative);
    ASSERT_EQ(res.size(), profileRelative.chargingSchedule.chargingSchedulePeriod.size());

    res_session = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res_session.size(), profileRelative.chargingSchedule.chargingSchedulePeriod.size());

    // the session start should change the result
    EXPECT_NE(res, res_session);

    const auto* entry = &res[0];
    EXPECT_EQ(entry->start, "2024-01-01T12:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:28:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res[1];
    EXPECT_EQ(entry->start, "2024-01-01T12:28:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:43:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res[2];
    EXPECT_EQ(entry->start, "2024-01-01T12:43:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:58:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res_session[0];
    EXPECT_EQ(entry->start, "2024-01-01T12:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:25:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res_session[1];
    EXPECT_EQ(entry->start, "2024-01-01T12:25:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:40:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res_session[2];
    EXPECT_EQ(entry->start, "2024-01-01T12:40:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:55:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    EXPECT_TRUE(validate_profile_result(res));
    EXPECT_TRUE(validate_profile_result(res_session));

    // during
    now = DateTime("2024-01-01T12:40:00Z");
    session_start = DateTime("2024-01-01T12:38:00Z");

    // expecting 3 periods to be included
    res = calculate_profile(now, end, std::nullopt, profileRelative);
    ASSERT_EQ(res.size(), 3);

    res_session = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res_session.size(), 3);

    entry = &res_session[0];
    EXPECT_EQ(entry->start, "2024-01-01T12:38:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:08:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res_session[1];
    EXPECT_EQ(entry->start, "2024-01-01T13:08:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:23:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res_session[2];
    EXPECT_EQ(entry->start, "2024-01-01T13:23:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:38:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    // the session start should change the result
    EXPECT_NE(res, res_session);
    EXPECT_TRUE(validate_profile_result(res));
    EXPECT_TRUE(validate_profile_result(res_session));

    // during - a bit later
    now = DateTime("2024-01-01T13:10:00Z");

    // expecting 3 periods for no session and 2 periods when there is an existing session
    res = calculate_profile(now, end, std::nullopt, profileRelative);
    ASSERT_EQ(res.size(), 3);

    res_session = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res_session.size(), 2);

    entry = &res_session[0];
    EXPECT_EQ(entry->start, "2024-01-01T13:08:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:23:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res_session[1];
    EXPECT_EQ(entry->start, "2024-01-01T13:23:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T13:38:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    // the session start should change the result
    EXPECT_NE(res, res_session);
    EXPECT_TRUE(validate_profile_result(res));
    EXPECT_TRUE(validate_profile_result(res_session));

    // after
    now = DateTime("2024-01-01T14:02:00Z");
    session_start = DateTime("2024-01-01T14:01:00Z");

    // expecting no periods to be included
    res = calculate_profile(now, end, std::nullopt, profileRelative);
    ASSERT_EQ(res.size(), 0);

    res_session = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res_session.size(), 0);

    // the session start should not change the result
    EXPECT_EQ(res, res_session);
    EXPECT_TRUE(validate_profile_result(res));
}

TEST(ProfileTestsA, calculateProfileRelativeLimited) {
    // valid 12:00 to 14:00 for 1 hour

    // before start
    DateTime session_start("2024-01-01T08:10:00Z");
    DateTime now(session_start.to_time_point() + minutes(2));
    DateTime end(now.to_time_point() + minutes(20));

    // expecting no periods
    auto res = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res.size(), 0);

    // just before start
    session_start = DateTime("2024-01-01T11:55:00Z");
    now = DateTime(session_start.to_time_point() + minutes(2));
    end = DateTime(now.to_time_point() + minutes(20));

    // expecting a single period
    res = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res.size(), 1);

    const auto* entry = &res[0];

    EXPECT_EQ(entry->start, "2024-01-01T12:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:25:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    EXPECT_TRUE(validate_profile_result(res));

    // during A
    now = DateTime(session_start.to_time_point() + minutes(25));
    end = DateTime(now.to_time_point() + minutes(20));

    // expecting 2 periods to be included
    res = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res.size(), 3);

    entry = &res[0];
    EXPECT_EQ(entry->start, "2024-01-01T12:00:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:25:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res[1];
    EXPECT_EQ(entry->start, "2024-01-01T12:25:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:40:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res[2];
    EXPECT_EQ(entry->start, "2024-01-01T12:40:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:55:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    EXPECT_TRUE(validate_profile_result(res));

    // during B
    now = DateTime(session_start.to_time_point() + minutes(35));
    end = DateTime(now.to_time_point() + minutes(20));

    // expecting 2 periods to be included
    res = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res.size(), 2);

    entry = &res[0];
    EXPECT_EQ(entry->start, "2024-01-01T12:25:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:40:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    entry = &res[1];
    EXPECT_EQ(entry->start, "2024-01-01T12:40:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T12:55:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    EXPECT_TRUE(validate_profile_result(res));

    // during C
    session_start = DateTime("2024-01-01T13:20:00Z");
    now = DateTime(session_start.to_time_point() + minutes(35));
    end = DateTime(now.to_time_point() + minutes(20));

    // expecting 1 period to be included
    res = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res.size(), 1);

    entry = &res[0];
    EXPECT_EQ(entry->start, "2024-01-01T13:50:00Z");
    EXPECT_EQ(entry->end, "2024-01-01T14:00:00Z");
    EXPECT_EQ(entry->limit, profileRelative.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_FALSE(entry->number_phases);
    EXPECT_EQ(entry->stack_level, profileRelative.stackLevel);

    EXPECT_TRUE(validate_profile_result(res));

    // after
    session_start = DateTime("2024-01-01T14:01:00Z");
    now = DateTime(session_start.to_time_point() + minutes(2));
    end = DateTime(now.to_time_point() + minutes(20));

    // expecting no periods to be included
    res = calculate_profile(now, end, session_start, profileRelative);
    ASSERT_EQ(res.size(), 0);
}

} // namespace
