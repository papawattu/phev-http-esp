/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include "esp_event.h"
#include "freertos/event_groups.h"
#include <esp_log.h>
#include <esp_system.h>
#include "esp_vfs.h"
#include <esp_spiffs.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include <esp_http_server.h>
#include "phev_service.h"
#include "phev_setup.h"
#include "captdns.h"

#include "tcp_client.h"
#include "wifi_client.h"
#include "msg_utils.h"
#include "msg_tcpip.h"

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 * The examples use simple WiFi configuration that you can set via
 * 'make menuconfig'.
 * If you'd rather not, just change the below entries to strings
 * with the config you want -
 * ie. #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

const static int CONNECTED_BIT = BIT0;
const static int START_BIT = BIT1;
const static int REGISTERED_BIT = BIT2;

static const char *TAG = "APP";

static EventGroupHandle_t main_event_group;

typedef struct httpSettings_t
{
    char *host;
    uint16_t port;
    httpd_handle_t server;
    int (*connect)(uint16_t);
    void *ctx;
} httpSettings_t;
typedef struct http_ctx_t
{
    uint16_t port;
    httpd_handle_t server;
    httpd_req_t * req;
    int (*connect)(uint16_t);
    void *ctx;
} http_ctx_t;

void createHandlers(httpd_handle_t server, connectionDetails_t * *);
phevServiceCtx_t *createServiceCtx(httpd_handle_t server, const uint8_t *mac, bool registerDevice,const char * host, uint16_t port);
void msg_http_incomingHandlerAsync(messagingClient_t *client, message_t *message);
/* An HTTP GET handler */

#define BUFFER_SIZE 1024
#define MIME_TYPE_JSON "application/json"
#define MAX_HTTP_RECV_BUFFER_SIZE 2048

phevServiceCtx_t *serviceCtx = NULL;

esp_err_t phev_http_operation_handler(httpd_req_t *req)
{
    const char *OK = "{ \"status\" : \"ok\" }";

    phevServiceCtx_t * ctx = (phevServiceCtx_t *) req->user_ctx;

    LOG_I(TAG,"Correct ctx %s",(ctx == serviceCtx ? "Correct" : "Incorrect"));
    
    size_t recv_size = req->content_len;

    char *content = malloc(recv_size);

    if (!content)
    {
        LOG_E(TAG, "Cannot allocate memory for http receive buffer");
        return ESP_FAIL;
    }

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    { /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    message_t *message = msg_utils_createMsg((uint8_t *)content, recv_size);

    msg_http_incomingHandlerAsync(serviceCtx->pipe->pipe->in, message);

    httpd_resp_set_type(req, MIME_TYPE_JSON);
    httpd_resp_send(req, OK, strlen(OK));

    return ESP_OK;
}

esp_err_t phev_http_get_config_handler(httpd_req_t *req)
{
    const char *OK = "{ \"status\" : \"ok\" }";

    httpd_resp_set_type(req, MIME_TYPE_JSON);
    httpd_resp_send(req, OK, strlen(OK));

    return ESP_OK;
}
esp_err_t phev_http_post_config_handler(httpd_req_t *req)
{
    const char *OK = "{ \"status\" : \"ok\" }";
    const char *ERROR = "{ \"status\" : \"error\" }";
    size_t recv_size = req->content_len;

    char *content = malloc(recv_size);

    if (!content)
    {
        LOG_E(TAG, "Cannot allocate memory for http receive buffer");
        return ESP_FAIL;
    }

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    { /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    connectionDetails_t * * details = req->user_ctx;
    
    (*details) = phev_setup_jsonToConnectionDetails(content);

    if (*details)
    {
        LOG_D(TAG, "SSID %s", (*details)->wifi.ssid);
        LOG_D(TAG, "Password %s", (*details)->wifi.password);
        LOG_D(TAG, "Host %s", (*details)->host);
        LOG_D(TAG, "Port %d", (*details)->port);
        
        wifi_conn_initStationAndWait((*details)->wifi.ssid, (*details)->wifi.password, false);
        httpd_resp_set_type(req, MIME_TYPE_JSON);
        httpd_resp_send(req, OK, strlen(OK));

        LOG_I(TAG, "Sent response");
        xEventGroupSetBits(main_event_group, CONNECTED_BIT);
        return ESP_OK;
    }
    else
    {
        LOG_E(TAG, "Connection details not set, could be invalid json");
        httpd_resp_set_type(req, MIME_TYPE_JSON);
        httpd_resp_send(req, ERROR, strlen(ERROR));
    }

    return ESP_OK;
}

esp_err_t phev_http_register_handler(httpd_req_t *req)
{
    const char *OK = "{ \"status\" : \"ok\" }";
    const char *ERROR = "{ \"status\" : \"error\" }";
    const uint8_t mac[] = {0,0,0,0,0,0};
    LOG_I(TAG, "Sent response");

    connectionDetails_t * details = (connectionDetails_t *)  req->user_ctx;
    
    serviceCtx = createServiceCtx(req->handle, mac, true, details->host,details->port);

    xEventGroupSetBits(main_event_group, START_BIT);

    xEventGroupWaitBits(main_event_group, REGISTERED_BIT,
                        false, true, portMAX_DELAY);
    
    httpd_resp_set_type(req, MIME_TYPE_JSON);
    httpd_resp_send(req, OK, strlen(OK));

    return ESP_OK;
}

esp_err_t phev_http_connect_handler(httpd_req_t *req)
{
    const char *OK = "{ \"status\" : \"ok\" }";
    const char *ERROR = "{ \"status\" : \"error\" }";
    const uint8_t mac[] = {0,0,0,0,0,0};
    
    connectionDetails_t * details = (connectionDetails_t *)  req->user_ctx;

    LOG_I(TAG,"host %s port %d",details->host,details->port);

    serviceCtx = createServiceCtx(req->handle, mac, false, details->host,details->port);
    
    xEventGroupSetBits(main_event_group, START_BIT);
    
    httpd_resp_set_type(req, MIME_TYPE_JSON);
    httpd_resp_send(req, OK, strlen(OK));

    LOG_I(TAG, "Sent response");

    return ESP_OK;
}

esp_err_t phev_http_stream_handler(httpd_req_t *req)
{
    LOG_I(TAG,"Stream handler called");
    httpd_resp_set_type(req, MIME_TYPE_JSON);

    ((http_ctx_t *) req->user_ctx)->req = req;

    while(((http_ctx_t *) req->user_ctx)->req);
    return ESP_OK;
}
esp_err_t phev_http_status_handler(httpd_req_t *req)
{
    const char * STATUS_ERROR = "{ \"error\" : \"Cannot get status\" }";
    phevServiceCtx_t * ctx = (phevServiceCtx_t *) req->user_ctx;

    LOG_I(TAG,"Correct ctx %s",(ctx == serviceCtx ? "Correct" : "Incorrect"));

    if (serviceCtx)
    {
        const char *buffer = phev_service_statusAsJson(serviceCtx);
        //LOG_I(TAG, "Battery level %d", phev_model_getRegister(serviceCtx->model, 29)->data[0]);
        LOG_I(TAG, "Status request");
        if(buffer) {
            LOG_I(TAG, "%s", buffer);
            httpd_resp_set_type(req, MIME_TYPE_JSON);
            httpd_resp_send(req, buffer, strlen(buffer));
        } else {
            httpd_resp_set_type(req, MIME_TYPE_JSON);
            httpd_resp_send(req, STATUS_ERROR, strlen(STATUS_ERROR));
        }
        
    }
    else
    {
        httpd_resp_set_type(req, MIME_TYPE_JSON);
        httpd_resp_send(req, "Error cant get status no context", strlen("Error cant get status no context"));
    }

    return ESP_OK;
}

esp_err_t phev_http_get_handler(httpd_req_t *req)
{
    char *filename;
    asprintf(&filename, "/spiffs%s", req->uri);
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }

    char buffer[BUFFER_SIZE];

    size_t len = fread(buffer, 1, BUFFER_SIZE, f);

    ESP_LOGI(TAG, "Read length %d", len);

    httpd_resp_send_chunk(req, buffer, len);

    while (len > 0)
    {
        ESP_LOGI(TAG, "Chunk length left %d", len);

        len = fread(buffer, 1, BUFFER_SIZE, f);

        if (len > 0)
        {
            httpd_resp_send_chunk(req, buffer, len);
            ESP_LOGI(TAG, "Written %d bytes", len);
        }
    }
    httpd_resp_send_chunk(req, buffer, 0);
    fclose(f);

    return ESP_OK;
}

esp_err_t phev_http_captdns_handler(httpd_req_t *req)
{

    //const static char http_header_redirect_to_setup[] = "HTTP/1.1 302 Found\r\nContent-Type: text/plain\r\nContent-length: 0\r\nLocation: http://setup.yourdevice.com/\r\nConnection: close\r\n\r\n";
    
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_hdr(req, "Location", "http://phevremote.com/index.html");
    
    httpd_resp_send(req, "\r\n", 2);
    
    return ESP_OK;
}
esp_err_t phev_http_root_handler(httpd_req_t *req)
{
    FILE *f = fopen("/spiffs/index.html", "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }

    char buffer[BUFFER_SIZE];

    size_t len = fread(buffer, 1, BUFFER_SIZE, f);

    ESP_LOGI(TAG, "Read length %d", len);

    httpd_resp_send_chunk(req, buffer, len);

    while (len > 0)
    {
        ESP_LOGI(TAG, "Chunk length left %d", len);

        len = fread(buffer, 1, BUFFER_SIZE, f);

        if (len > 0)
        {
            httpd_resp_send_chunk(req, buffer, len);
            ESP_LOGI(TAG, "Written %d bytes", len);
        }
    }
    httpd_resp_send_chunk(req, buffer, 0);
    fclose(f);

    return ESP_OK;
}
httpd_uri_t phev_http_root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = phev_http_root_handler,
};
httpd_handle_t start_webserver(connectionDetails_t * * details)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 20;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        createHandlers(server, details);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    bool *wifi = (bool *)ctx;

    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "Got IP: '%s'",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        *wifi = true;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        ESP_ERROR_CHECK(esp_wifi_connect());

        /* Stop the web server */
        *wifi = false;
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void *arg)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void initialise_spiffs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

void createHandlers(httpd_handle_t server, connectionDetails_t * * details)
{
    const char *path = "/spiffs/";
    struct dirent *ent;
    DIR *dir = opendir(path);
    if (!dir)
    {
        ESP_LOGE(TAG, "Error opening directory\n");
        return;
    }
    while ((ent = readdir(dir)) != NULL)
    {
        char name[100];
        ESP_LOGI(TAG, "Name %s", ent->d_name);
        sprintf(name, "/%s", ent->d_name);
        httpd_uri_t phev_http_handler = {
            .uri = strdup(name),
            .method = HTTP_GET,
            .handler = phev_http_get_handler,
        };

        httpd_register_uri_handler(server, &phev_http_handler);
    }

    httpd_register_uri_handler(server, &phev_http_root);
    httpd_uri_t phev_http_get_config = {
        .uri = "/config",
        .method = HTTP_GET,
        .handler = phev_http_get_config_handler,
        .user_ctx = NULL,
    };

    httpd_register_uri_handler(server, &phev_http_get_config);
    httpd_uri_t phev_http_post_config = {
        .uri = "/config",
        .method = HTTP_POST,
        .handler = phev_http_post_config_handler,
        .user_ctx = details,
    };

    httpd_register_uri_handler(server, &phev_http_post_config);

    httpd_uri_t phev_http_captdns = {
        .uri = "/generate_204",
        .method = HTTP_GET,
        .handler = phev_http_captdns_handler,
        .user_ctx = NULL,
    };

    httpd_register_uri_handler(server, &phev_http_captdns);

}

int msg_http_connect(messagingClient_t *client)
{
    LOG_I(TAG, "Http connect");
    LOG_D(TAG, "Client %p", client);
    http_ctx_t *ctx = (http_ctx_t *)client->ctx;
    phevServiceCtx_t *service = (phevServiceCtx_t *)ctx->ctx;
    LOG_D(TAG, "Ctx %p", ctx);

    if (!ctx->server)
    {
        ctx->server = start_webserver(NULL);
    }

    if (ctx->server)
    {
        httpd_uri_t phev_http_operation = {
            .uri = "/operation",
            .method = HTTP_POST,
            .handler = phev_http_operation_handler,
            .user_ctx = service,
        };

        httpd_register_uri_handler(ctx->server, &phev_http_operation);

        httpd_uri_t phev_http_status = {
            .uri = "/status",
            .method = HTTP_GET,
            .handler = phev_http_status_handler,
            .user_ctx = client->ctx,
        };

        httpd_register_uri_handler(ctx->server, &phev_http_status);

        httpd_uri_t phev_http_stream = {
            .uri = "/stream",
            .method = HTTP_GET,
            .handler = phev_http_stream_handler,
            .user_ctx = client,
        };

        httpd_register_uri_handler(ctx->server, &phev_http_stream);

        client->connected = 1;
        return 0;
    }
    else
    {
        LOG_E(TAG, "Server not started");
        client->connected = 0;
        return -1;
    }
}
void msg_http_incomingHandlerAsync(messagingClient_t *client, message_t *message)
{
    msg_core_call_subs(client, message);
}
message_t *msg_http_incomingHandler(messagingClient_t *client)
{
    return NULL;
}

void msg_http_outgoingHandler(messagingClient_t *client, message_t *message)
{
    //ESP_LOG_BUFFER_HEXDUMP(TAG,message->data,message->length,ESP_LOG_INFO);
    if(((http_ctx_t *) client->ctx)->req)
    {
        LOG_I(TAG,"Stream %s",(char *) message->data);
        httpd_resp_send_chunk(((http_ctx_t *) client->ctx)->req, (char *) message->data, message->length);
    } else {
      //  LOG_I(TAG,"No Stream");
        
    }
    
}

messagingClient_t *msg_http_createHttpClient(httpSettings_t settings)
{
    messagingSettings_t clientSettings;

    http_ctx_t *ctx = malloc(sizeof(http_ctx_t));

    clientSettings.incomingHandler = msg_http_incomingHandler;
    clientSettings.outgoingHandler = msg_http_outgoingHandler;

    clientSettings.connect = msg_http_connect;

    clientSettings.ctx = (void *)ctx;
    clientSettings.start = NULL;
    clientSettings.stop = NULL;
    ctx->server = settings.server;
    ctx->req = NULL;

    return msg_core_createMessagingClient(clientSettings);
}

void yieldHandler(void)
{
    vTaskDelay(100 / portTICK_PERIOD_MS);
}
void phev_service_complete_callback(phevRegisterCtx_t *ctx)
{
    LOG_I(TAG, "Device Registered");
    xEventGroupSetBits(main_event_group, REGISTERED_BIT);
}

phevServiceCtx_t *createServiceCtx(httpd_handle_t server, const uint8_t *mac, bool registerDevice, const char * host, uint16_t port)
{
    httpSettings_t inSettings = {
        .server = server,
    };
    tcpIpSettings_t outSettings = {
        .connect = tcp_client_connectSocket,
        .read = tcp_client_read,
        .write = tcp_client_write,
        .host = host,
        .port = port,
    };
    messagingClient_t *in = msg_http_createHttpClient(inSettings);
    messagingClient_t *out = msg_tcpip_createTcpIpClient(outSettings);

    LOG_I(TAG, "Created clients");

    if (registerDevice)
    {

        LOG_I(TAG, "Registering");

        phevServiceSettings_t settings = {
            .in = in,
            .out = out,
            .mac = mac,
            .registerDevice = true,
            .eventHandler = NULL,
            .errorHandler = NULL,
            .yieldHandler = yieldHandler,
        };

        LOG_I(TAG, "PHEV service creating");

        serviceCtx = phev_service_create(settings);

        LOG_I(TAG, "PHEV register");

        phevRegisterCtx_t *regCtx = phev_service_register(&mac, serviceCtx, phev_service_complete_callback);
    }
    else
    {

        LOG_I(TAG, "Not Registering");

        phevServiceSettings_t settings = {
            .in = in,
            .out = out,
            .mac = mac,
            .registerDevice = false,
            .eventHandler = NULL,
            .errorHandler = NULL,
            .yieldHandler = yieldHandler,
        };

        LOG_I(TAG, "PHEV service create");

        serviceCtx = phev_service_create(settings);
    }
    return serviceCtx;
}

void main_loop(void * args)
{
    phev_service_start(serviceCtx);
}
void app_main()
{

    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);
    tcpip_adapter_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip,1,2,3,4);
    IP4_ADDR(&ip_info.gw,1,2,3,4);
    IP4_ADDR(&ip_info.netmask,255,255,255,0);
    tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ip_info);
    tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);
    
    LOG_I(TAG, "Init SPIFFS");
    initialise_spiffs();
    connectionDetails_t * details;
    
    httpd_handle_t server = start_webserver(&details);
    
    wifi_client_setup(server);

    main_event_group = xEventGroupCreate();
    
    wifi_ap_init(server);

    captdnsInit();


    xEventGroupWaitBits(main_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    httpd_uri_t phev_http_register = {
        .uri = "/register",
        .method = HTTP_POST,
        .handler = phev_http_register_handler,
        .user_ctx = details,
    };

    httpd_register_uri_handler(server, &phev_http_register);
    
        httpd_uri_t phev_http_connect = {
        .uri = "/connect",
        .method = HTTP_POST,
        .handler = phev_http_connect_handler,
        .user_ctx = details,
    };

    httpd_register_uri_handler(server, &phev_http_connect);
    
    xEventGroupWaitBits(main_event_group, START_BIT,
                        false, true, portMAX_DELAY);

    LOG_I(TAG, "PHEV service start");

    xTaskCreate(main_loop, "main_task", 4096, NULL, 5, NULL);  
    
}
