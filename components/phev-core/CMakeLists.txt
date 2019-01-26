register_component()

SET(COMPONENT_REQUIRES "msg-core")

add_library(phev_core STATIC
    phev-core/src/phev_register.c
    phev-core/src/phev_pipe.c
    phev-core/src/phev_core.c
    phev-core/src/phev_service.c
    phev-core/src/phev_model.c
)

target_include_directories(phev_core PUBLIC phev-core/include)