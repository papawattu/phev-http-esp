set(COMPONENT_ADD_INCLUDEDIRS phev-core/include)
set(COMPONENT_SRCS "phev-core/src/phev_register.c"
    "phev-core/src/phev_pipe.c"
    "phev-core/src/phev_core.c"
    "phev-core/src/phev_service.c"
    "phev-core/src/phev_model.c"
    "phev-core/src/phev_config.c"
)

set(COMPONENT_REQUIRES "msg-core" "json")
register_component()