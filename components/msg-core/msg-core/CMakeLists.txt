project("Message Core" C)
cmake_minimum_required(VERSION 3.1)

set (CMAKE_C_STANDARD 11)

add_library(msg_core STATIC
    src/msg_core.c
    src/msg_gcp_mqtt.c
    src/msg_mqtt.c
    src/msg_pipe_splitter.c
    src/msg_pipe.c 
    src/msg_tcpip.c 
    src/msg_utils.c
)

option (LOG_LEVEL "Log level" 0)
include(CTest)

add_subdirectory(external) 

add_subdirectory(test)

target_include_directories(msg_core PUBLIC include)

