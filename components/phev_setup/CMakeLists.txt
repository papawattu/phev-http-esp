set(COMPONENT_SRCDIRS "./src")
set(COMPONENT_ADD_INCLUDEDIRS "./include")

set(COMPONENT_REQUIRES logger phev-core esp_http_server wifi_client nvs_flash esp_http_client)
register_component()