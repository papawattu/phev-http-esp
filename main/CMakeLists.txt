set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES logger phev-core spiffs esp_http_server nvs_flash msg-core tcp_client wifi_client phev_setup)
register_component()
