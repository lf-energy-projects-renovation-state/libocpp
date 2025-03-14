// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ocpp/common/message_queue.hpp>
#include <ocpp/v16/messages/Authorize.hpp>
#include <ocpp/v16/messages/MeterValues.hpp>
#include <ocpp/v16/messages/SecurityEventNotification.hpp>
#include <ocpp/v16/messages/StartTransaction.hpp>

namespace ocpp {

namespace v16 {

/************************************************************************************************
 * ControlMessage
 *
 * Test implementations of ControlMessage template
 */
class ControlMessageV16Test : public ::testing::Test {

protected:
};

TEST_F(ControlMessageV16Test, test_is_transactional) {

    EXPECT_TRUE(is_transaction_message((ControlMessage<v16::MessageType>{
        Call<v16::StartTransactionRequest>{
            v16::StartTransactionRequest{}}}.messageType)));
    EXPECT_TRUE(is_transaction_message((ControlMessage<v16::MessageType>{
        Call<v16::StopTransactionRequest>{
            v16::StopTransactionRequest{}}}.messageType)));
    EXPECT_TRUE(is_transaction_message(ControlMessage<v16::MessageType>{
        Call<v16::SecurityEventNotificationRequest>{v16::SecurityEventNotificationRequest{}}}
                                           .messageType));
    EXPECT_TRUE(is_transaction_message(
        ControlMessage<v16::MessageType>{Call<v16::MeterValuesRequest>{v16::MeterValuesRequest{}}}.messageType));
    EXPECT_TRUE(!is_transaction_message(
        ControlMessage<v16::MessageType>{Call<v16::AuthorizeRequest>{v16::AuthorizeRequest{}}}.messageType));
}

TEST_F(ControlMessageV16Test, test_is_transactional_update) {

    EXPECT_TRUE(!(ControlMessage<v16::MessageType>{Call<v16::StartTransactionRequest>{v16::StartTransactionRequest{}}})
                     .is_transaction_update_message());
    EXPECT_TRUE(!(ControlMessage<v16::MessageType>{Call<v16::StopTransactionRequest>{v16::StopTransactionRequest{}}})
                     .is_transaction_update_message());
    EXPECT_TRUE(!(ControlMessage<v16::MessageType>{
                      Call<v16::SecurityEventNotificationRequest>{v16::SecurityEventNotificationRequest{}}})
                     .is_transaction_update_message());
    EXPECT_TRUE((ControlMessage<v16::MessageType>{Call<v16::MeterValuesRequest>{v16::MeterValuesRequest{}}})
                    .is_transaction_update_message());

    EXPECT_TRUE(!(ControlMessage<v16::MessageType>{Call<v16::AuthorizeRequest>{v16::AuthorizeRequest{}}})
                     .is_transaction_update_message());
}

} // namespace v16
} // namespace ocpp
