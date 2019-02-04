#ifndef SETUP_UI_H_
#define SETUP_UI_H_

#include "phev_core.h"
#include "phev_config.h"
#include "esp_http_server.h"

#define MAX_HTTP_RECV_BUFFER 1024

#define SETUP_EMAIL "email"

#define SETUP_CONNECTION_CONFIG_JSON "carConnection"
#define SETUP_CONNECTION_CONFIG_HOST "host"
#define SETUP_CONNECTION_CONFIG_PORT "port"
#define SETUP_CONNECTION_CONFIG_SSID "ssid"
#define SETUP_CONNECTION_CONFIG_PASSWORD "password"

#define SETUP_PPP_CONFIG_JSON "pppConnection"
#define SETUP_PPP_CONFIG_USER "user"
#define SETUP_PPP_CONFIG_PASSWORD "password"
#define SETUP_PPP_CONFIG_APN "apn"

#define DEFAULT_CAR_HOST_IP "192.168.8.46"
#define DEFAULT_CAR_HOST_PORT 8080
#define DEFAULT_PPP_USER "eesecure"
#define DEFAULT_PPP_PASSWORD "secure"
#define DEFAULT_PPP_APN "everywhere"

#define GCP_PROJECTID "projectId"
#define GCP_REGISTRY "registry"
#define GCP_LOCATION "location"

#define GCP_EVENTS_TOPIC "eventsTopic"
#define GCP_STATE_TOPIC "stateTopic"
#define GCP_COMMANDS_TOPIC "commandsTopic"
#define GCP_CONFIG_TOPIC "configTopic"
typedef struct connectionDetails_t {
    char * email;
    phevWifi_t wifi;
    char * host;
    uint16_t port;
    char * pppUser;
    char * pppPassword;
    char * pppAPN;
} connectionDetails_t;

typedef void phevStore_t;

typedef struct phevSetupConnectionConfig_t 
{
    phevWifi_t wifi;
    char * host;
    uint16_t port;
    char * pppUser;
    char * pppPassword;
    char * pppAPN;
    char * mqttUri;
    char * gcpProjectId;
    char * gcpLocation;
    char * gcpRegistry;
    char * eventsTopic;
    char * stateTopic;
    char * commandsTopic;
    char * configTopic;
} phevSetupConnectionConfig_t;

httpd_handle_t * phev_setup_startWebserver(phevStore_t *);
void phev_setup_stopWebserver(httpd_handle_t server);
void phev_setup_startPPPConnection(phevStore_t * store);
void phev_setup_startWifiConnection(phevStore_t * store);
void phev_setup_register(phevStore_t * store);
connectionDetails_t * phev_setup_jsonToConnectionDetails(const char * config);

#endif 