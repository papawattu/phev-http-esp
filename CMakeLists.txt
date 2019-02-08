# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(phev-http-esp)
set(CMAKE_C_FLAGS "-mlongcalls" CACHE STRING "C Compiler Base Flags")
#set(COMMON_FLAGS "-ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls" CACHE STRING "Common flags")
set(COMPONENT_REQUIRES "phev-core spiffs esp_http_server nvs_flash")
