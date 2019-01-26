#ifndef _PHEV_REGISTER_H_
#define _PHEV_REGISTER_H_

#include "msg_pipe.h"
#include "phev_pipe.h"

#define MAC_ADDR_SIZE 6

typedef struct phevRegisterCtx_t phevRegisterCtx_t;

typedef void (* phevRegistrationComplete_t)(phevRegisterCtx_t *);

typedef struct phevRegisterSettings_t {
    phev_pipe_ctx_t * pipe;
    phevPipeEventHandler_t eventHandler;
    uint8_t mac[MAC_ADDR_SIZE];
    phevRegistrationComplete_t complete;
    phevErrorHandler_t errorHandler;
    void * ctx;
} phevRegisterSettings_t;

typedef struct phevRegisterCtx_t {
    phev_pipe_ctx_t * pipe;
    uint8_t mac[MAC_ADDR_SIZE];
    phevRegistrationComplete_t complete;
    phevErrorHandler_t errorHandler;
    char * vin;
    bool startAck;
    bool aaAck;
    bool registrationRequest;
    bool ecu;
    bool remoteSecurity;
    bool registrationAck;
    bool registrationComplete;
    void * ctx;
        
} phevRegisterCtx_t;

phevRegisterCtx_t * phev_register_init(phevRegisterSettings_t);
void phev_register_start(phevRegisterCtx_t *);
int phev_register_eventHandler(phev_pipe_ctx_t * ctx, phevPipeEvent_t * event);

#endif