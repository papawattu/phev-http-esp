#include "unity.h"

#include "msg_core.h"
#include "msg_utils.h"
#include "msg_mqtt.h"
#include "logger.h"

#define TOPIC "topic"
#define MSG_ID 1234
#define DATA_LEN 4

uint8_t DATA[] = {1,2,3,4};

static int msg_mqtt_mock_init_Called = 0;
static int msg_mqtt_mock_start_Called = 0;
static int msg_mqtt_mock_publish_Called = 0;

handle_t msg_mqtt_mock_init(const msg_mqtt_config_t *config) 
{
    msg_mqtt_mock_init_Called ++;
    msg_mqtt_t * mqtt = config->user_context;
    return NULL;
}
msg_mqtt_err_t msg_mqtt_mock_start(handle_t client) 
{
    msg_mqtt_mock_start_Called++;
    return MQTT_OK;
}

int msg_mqtt_mock_publish(handle_t client, const char *topic, const char *data, int len, int qos, int retain)
{
    msg_mqtt_mock_publish_Called++;

    TEST_ASSERT_EQUAL_STRING(TOPIC,topic);
    TEST_ASSERT_EQUAL(4, len);
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(&DATA,data,4);
    
    return MSG_ID;
}

    

void test_msg_mqtt_start(void)
{
    msg_mqtt_t mqtt = {
        .init = msg_mqtt_mock_init,
        .start = msg_mqtt_mock_start
    };
    msg_mqtt_settings_t settings = {
        .host = "www.test.com",
        .port = 8883,
        .mqtt = &mqtt

    };
    handle_t handle = msg_mqtt_start(&settings);

    TEST_ASSERT_EQUAL(1,msg_mqtt_mock_init_Called);
    TEST_ASSERT_EQUAL(1,msg_mqtt_mock_start_Called);
    
}

void test_msg_mqtt_publish(void)
{
    msg_mqtt_t mqtt = {
        .publish = &msg_mqtt_mock_publish
    };

    message_t * message = malloc(sizeof(message_t));

    message->data = malloc(4);
    message->length = 4;
    message->topic = strdup(TOPIC);

    memcpy(message->data, &DATA, 4);
    message_t * out = malloc(sizeof(message_t));
    out->data = malloc(sizeof(DATA));
    memcpy(out->data,DATA,sizeof(DATA));
    out->length = sizeof(DATA);
    
    //msg_utils_copyMsg_ExpectAndReturn(&message,out);
    
    int msgId = msg_mqtt_publish(&mqtt, TOPIC, message);

    TEST_ASSERT_EQUAL(1,msg_mqtt_mock_publish_Called);
    TEST_ASSERT_EQUAL(MSG_ID,msgId);
}

static int msg_mqtt_mock_incoming_Called = 0;

messagingClient_t * msg_mqtt_msg_client = NULL;

void msg_mqtt_mock_incoming(messagingClient_t * client, message_t * message) 
{
    msg_mqtt_mock_incoming_Called ++;

    TEST_ASSERT_EQUAL(DATA_LEN, message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(DATA, message->data,DATA_LEN);
    TEST_ASSERT_EQUAL(msg_mqtt_msg_client, client);

}

void test_msg_mqtt_event_data(void)
{

    messagingClient_t client = {

    };

    msg_mqtt_msg_client = &client;

    msg_mqtt_t mqtt = {
        .incoming_cb = &msg_mqtt_mock_incoming,
        .client = msg_mqtt_msg_client
    };

    
    mqtt_event_t event = {
        .event_id = MSG_MQTT_EVENT_DATA,
        .data = DATA,
        .data_len = DATA_LEN,
        .topic = TOPIC,
        .topic_len = 5,
        .user_context = &mqtt
    };

    dataEvent(&event);

    TEST_ASSERT_EQUAL(1,msg_mqtt_mock_incoming_Called);

}