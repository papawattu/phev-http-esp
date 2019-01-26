set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES "phev-core spiffs esp_http_server nvs_flash")
register_component()
