set(COMPONENT_ADD_INCLUDEDIRS msg-core/include)
set(COMPONENT_SRCS "msg-core/src/msg_core.c"
    "msg-core/src/msg_gcp_mqtt.c"
    "msg-core/src/msg_mqtt.c"
    "msg-core/src/msg_pipe_splitter.c"
    "msg-core/src/msg_pipe.c" 
    "msg-core/src/msg_tcpip.c" 
    "msg-core/src/msg_utils.c"

)

register_component()
