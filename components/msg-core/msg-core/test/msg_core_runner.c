#include "unity.h"
#include "test_msg_core.c"
#include "test_msg_gcp_mqtt.c"
#include "test_msg_mqtt.c"
#include "test_msg_pipe_splitter.c"
#include "test_msg_pipe.c"
#include "test_msg_tcpip.c"
#include "test_msg_utils.c"

int main()
{
    UNITY_BEGIN();

//  MSG_CORE

    RUN_TEST(test_msg_core_bootstrap);
    RUN_TEST(test_msg_core_publish_returns_zero);
    RUN_TEST(test_msg_core_publish_calls_outgoing_handler);
    RUN_TEST(test_msg_core_register_handlers_returns_zero);
    RUN_TEST(test_msg_core_register_handlers_can_be_called);
    RUN_TEST(test_msg_core_published_message_data);
    RUN_TEST(test_msg_core_create_messaging_client);
    RUN_TEST(test_msg_core_create_messaging_client_start);
    RUN_TEST(test_msg_core_create_messaging_client_stop);
    RUN_TEST(test_msg_core_create_messaging_client_connect);
    RUN_TEST(test_msg_core_messaging_client_start);
    RUN_TEST(test_msg_core_messaging_client_stop);
    RUN_TEST(test_msg_core_messaging_client_connect);
    RUN_TEST(test_msg_core_messaging_client_connect_default);
    RUN_TEST(test_msg_core_create_messaging_client_default_settings);
    RUN_TEST(test_msg_core_create_messaging_client_default_settings_call_start);
    RUN_TEST(test_msg_core_create_messaging_client_default_settings_call_stop);
    RUN_TEST(test_msg_core_create_messaging_client_default_settings_call_connect);

// MSG_GCP_MQTT

//    RUN_TEST(test_msg_gcp_mqtt_createGcpClient);

// MSG_MQTT

    RUN_TEST(test_msg_mqtt_event_data);
    RUN_TEST(test_msg_mqtt_publish);
    RUN_TEST(test_msg_mqtt_start);
    RUN_TEST(test_msg_core_messaging_pub_sub);

// MSG_PIPE_SPLITTER

    RUN_TEST(test_msg_pipe_splitter_should_aggregate_message_bundle);
    RUN_TEST(test_msg_pipe_splitter_should_split_single);
    RUN_TEST(test_msg_pipe_splitter_should_split_double);

// MSG_PIPE

    RUN_TEST(test_msg_pipe_should_call_start_incoming);
    RUN_TEST(test_msg_pipe_should_call_connect_incoming);
    RUN_TEST(test_msg_pipe_should_call_start_outgoing);
    RUN_TEST(test_msg_pipe_should_call_connect_outgoing);
    RUN_TEST(test_msg_pipe_should_not_call_connect_outgoing);
    RUN_TEST(test_msg_pipe_should_connect_on_publish);
    RUN_TEST(test_msg_pipe_should_set_in_and_out_clients);
    RUN_TEST(test_msg_pipe_should_call_loop_for_both_clients);
//    RUN_TEST(test_msg_pipe_should_subscribe_both_clients);
    RUN_TEST(test_msg_pipe_should_publish_message_incoming);
    RUN_TEST(test_msg_pipe_should_publish_message_outgoing);
    RUN_TEST(test_msg_pipe_should_set_in_input_transformer_in_settings);
    RUN_TEST(test_msg_pipe_should_set_out_input_transformer_in_settings);
    RUN_TEST(test_msg_pipe_should_set_out_output_transformer);  
    RUN_TEST(test_msg_pipe_should_set_in_output_transformer);
    RUN_TEST(test_msg_pipe_should_set_up_in_splitter);
    RUN_TEST(test_msg_pipe_should_set_up_out_splitter);

//  MSG_TCPIP

    RUN_TEST(test_msg_tcpip_create_tcpip_client);
    RUN_TEST(test_msg_tcpip_client_connect);
    RUN_TEST(test_msg_tcpip_client_outgoing_handler);
    RUN_TEST(test_msg_tcpip_client_incoming_handler_no_message);
    RUN_TEST(test_msg_tcpip_client_incoming_handler);

// MSG_UTILS

    RUN_TEST(test_msg_utils_create_msg);
    RUN_TEST(test_msg_utils_copy_message);
    RUN_TEST(test_msg_utils_destroy_message);
    RUN_TEST(test_msg_utils_concat_messages);

    return UNITY_END();
}