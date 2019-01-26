#include "unity.h"

#include "msg_core.h"
#include "msg_mqtt.h"
#include "msg_gcp_mqtt.h"

char * msg_gcp_mqtt_dummyJwt(void) 
{
    return NULL;
}

char * msg_gcp_mqtt_createJwt(char *txt) 
{
    return "1234";
} 

void msg_gcp_mqtt_dummy(void)
{

}
void test_msg_gcp_mqtt_createGcpClient(void)
{

    msg_mqtt_t mqtt = {
        .init = &msg_gcp_mqtt_dummy,
        .start = &msg_gcp_mqtt_dummy,
        .publish = &msg_gcp_mqtt_dummy,
        .subscribe = &msg_gcp_mqtt_dummy
    };

    gcpSettings_t settings = {
        .host = "mqtt.googleapis.com",
        .port = 8883,
        .clientId = "projects/phev-db3fa/locations/us-central1/registries/my-registry/devices/my-device",
        .device = "my-device",
        .createJwt = msg_gcp_mqtt_createJwt,
        .mqtt = &mqtt,
        .projectId = "phev-db3fa",
        .eventTopic = "/devices/my-device/events",
        .stateTopic = "my-device-state"
    
    };
    
    messagingClient_t * client = msg_gcp_createGcpClient(settings);
    TEST_ASSERT_NOT_NULL(client);
    //TEST_ASSERT_EQUAL_STRING((const char *) "1.1.1.1", ((gcp_ctx_t *) client->ctx)->host);
}