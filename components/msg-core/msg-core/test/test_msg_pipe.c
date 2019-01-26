#include "unity.h"
#include "msg_core.h"
#include "msg_pipe_splitter.h"
#include "msg_pipe.h"
#include "logger.h"

static int msg_pipe_loopInStubCalled = 0;
static int msg_pipe_loopOutStubCalled = 0;

void msg_core_createMessagingClient_ExpectAndReturn(messagingSettings_t settings, messagingClient_t * client)
{
    //msg_core_createMessagingClient(settings);
}
void msg_pipe_loopInStub(messagingClient_t * ctx)
{
    msg_pipe_loopInStubCalled ++;
}
void msg_pipe_loopOutStub(messagingClient_t * ctx)
{
    msg_pipe_loopOutStubCalled ++;
} 

static int msg_pipe_startInStubNum = 0;

int msg_pipe_startInStub(messagingClient_t *client)
{
    msg_pipe_startInStubNum ++;
}
static int msg_pipe_connectInStubNum = 0;

int msg_pipe_connectInStub(messagingClient_t *client)
{
    msg_pipe_connectInStubNum ++;
}
static int msg_pipe_startOutStubNum = 0;

int msg_pipe_startOutStub(messagingClient_t *client)
{
    msg_pipe_startOutStubNum ++;
}
static int msg_pipe_connectOutStubNum = 0;

int msg_pipe_connectOutStub(messagingClient_t *client)
{
    msg_pipe_connectOutStubNum ++;
}
static int msg_pipe_subscribeInStubNum = 0;

void msg_pipe_subscribeInStub(messagingClient_t client, void * params, messagingSubscriptionCallback_t callback)
{
    msg_pipe_subscribeInStubNum++;
}
static int msg_pipe_subscribeOutStubNum = 0;

void msg_pipe_subscribeOutStub(messagingClient_t client, void * params, messagingSubscriptionCallback_t callback)
{
    msg_pipe_subscribeOutStubNum++;
} 
void test_msg_pipe_should_call_start_incoming()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
    };

    msg_pipe_startInStubNum = 0;
        
    msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(1,msg_pipe_startInStubNum);
    
}  
void test_msg_pipe_should_call_connect_incoming()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
    };

    msg_pipe_connectInStubNum = 0;
        
    msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(1,msg_pipe_connectInStubNum);
} 
void test_msg_pipe_should_call_start_outgoing()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
    };

    msg_pipe_startOutStubNum = 0;

    msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(1,msg_pipe_startOutStubNum);
    
} 
void test_msg_pipe_should_call_connect_outgoing()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
    };
    msg_pipe_connectOutStubNum = 0;

    msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(1,msg_pipe_connectOutStubNum);
} 
void test_msg_pipe_should_not_call_connect_outgoing()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .lazyConnect = 1,
    };

    msg_pipe_connectOutStubNum = 0;

    msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(0,msg_pipe_connectOutStubNum);
} 

static int msg_pipe_dummyPublishCalled = 0;

void msg_pipe_dummyPublish(messagingClient_t * client, message_t * message)
{

} 
void test_msg_pipe_should_connect_on_publish()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .lazyConnect = 1,
    };
    msg_pipe_connectOutStubNum = 0;
    
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(0, msg_pipe_connectOutStubNum);
    const uint8_t data[] = {1,2,3,4};

    message_t * message = malloc(sizeof(message_t));
    message->data = malloc(4);
    memcpy(message->data, data, 4);
    message->length = 4;
    
    msg_pipe_inboundSubscription(pipe_settings.out, ctx, message);

    TEST_ASSERT_EQUAL(1,msg_pipe_connectOutStubNum);
} 
void test_msg_pipe_should_set_in_and_out_clients()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .lazyConnect = 1,
    };
    msg_pipe_connectOutStubNum = 0;
 
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(pipe_settings.in,ctx->in);
    TEST_ASSERT_EQUAL(pipe_settings.out,ctx->out);
} 
void test_msg_pipe_should_call_loop_for_both_clients()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .lazyConnect = 1,
    };

    msg_pipe_connectOutStubNum = 0;
  
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    ctx->in->loop = msg_pipe_loopInStub;
    ctx->out->loop = msg_pipe_loopOutStub;

    ctx->loop(ctx);
    
    TEST_ASSERT_EQUAL(1,msg_pipe_loopInStubCalled);
    TEST_ASSERT_EQUAL(1,msg_pipe_loopOutStubCalled);
} /*
void test_msg_pipe_should_subscribe_both_clients()
{
    
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .lazyConnect = 1,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    ctx->loop(ctx);

    TEST_ASSERT_EQUAL(1,msg_pipe_subscribeInStubNum);
    TEST_ASSERT_EQUAL(1,msg_pipe_subscribeOutStubNum);
    
} */
static int msg_pipe_incomingPublishCalled = 0;
void msg_pipe_incomingPublish(messagingClient_t *client, message_t * message)
{
    msg_pipe_incomingPublishCalled ++;
    TEST_ASSERT_NOT_NULL(message);
} 
void test_msg_pipe_should_publish_message_incoming()
{ 
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .lazyConnect = 1,
    };

    msg_pipe_incomingPublishCalled = 0;
        
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    ctx->in->publish = msg_pipe_incomingPublish;

    const uint8_t data[] = {1,2,3,4};

    message_t * message = malloc(sizeof(message_t));
    message->data = malloc(4);
    memcpy(message->data, data, 4);
    message->length = 4;
    
    msg_pipe_outboundSubscription(pipe_settings.in, ctx, message);

    ctx->loop(ctx);

    TEST_ASSERT_EQUAL(1, msg_pipe_incomingPublishCalled);
    
} 
static int msg_pipe_outgoingPublishCalled = 0;
void msg_pipe_outgoingPublish(messagingClient_t *client, message_t * message)
{
    msg_pipe_outgoingPublishCalled ++;
    TEST_ASSERT_NOT_NULL(message);
}
void test_msg_pipe_should_publish_message_outgoing()
{ 

    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .lazyConnect = 1,
    };
        
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    ctx->out->publish = msg_pipe_outgoingPublish;

    msg_pipe_outgoingPublishCalled = 0;
        
    const uint8_t data[] = {1,2,3,4};

    message_t * message = malloc(sizeof(message_t));
    message->data = malloc(4);
    memcpy(message->data, data, 4);
    message->length = 4;
    
    msg_pipe_inboundSubscription(pipe_settings.out, ctx, message);
    
    ctx->loop(ctx);
    
    TEST_ASSERT_EQUAL(1, msg_pipe_outgoingPublishCalled);
    
} 
static int msg_pipe_inputTransformCalled = 0;
message_t * msg_pipe_inputTransform(message_t * message) 
{
    msg_pipe_inputTransformCalled ++;
    return message;
} 
void test_msg_pipe_should_set_in_input_transformer_in_settings()
{

    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    msg_pipe_chain_t chain = {
        .inputTransformer = msg_pipe_inputTransform,
    };    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .in_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(msg_pipe_inputTransform,ctx->in_chain->inputTransformer);
} 
void test_msg_pipe_should_set_out_input_transformer_in_settings()
{

    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_inputTransformCalled = 0;
    msg_pipe_chain_t chain = {
        .inputTransformer = msg_pipe_inputTransform,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .out_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(msg_pipe_inputTransform,ctx->out_chain->inputTransformer);
}
 
static int msg_pipe_outputTransformCalled = 0;
message_t * msg_pipe_outputTransform(message_t * message) 
{
    msg_pipe_outputTransformCalled ++;
    return message;
} 
void test_msg_pipe_should_set_out_output_transformer()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_outputTransformCalled = 0;
    msg_pipe_chain_t chain = {
        .outputTransformer = msg_pipe_outputTransform,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .out_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(msg_pipe_outputTransform,ctx->out_chain->outputTransformer);
} 
void test_msg_pipe_should_set_in_output_transformer()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_outputTransformCalled = 0;
    msg_pipe_chain_t chain = {
        .outputTransformer = msg_pipe_outputTransform,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .in_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(msg_pipe_outputTransform,ctx->in_chain->outputTransformer);
}  

static int msg_pipe_splitterCalled = 0;
message_t * msg_pipe_splitter_no_message(message_t * message)
{
    msg_pipe_splitterCalled ++;
    return NULL;
} 
void test_msg_pipe_should_set_up_in_splitter()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_splitterCalled = 0;
    
    msg_pipe_chain_t chain = {
        .splitter = msg_pipe_splitter_no_message,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .in_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(msg_pipe_splitter_no_message,ctx->in_chain->splitter);
}  
void test_msg_pipe_should_set_up_out_splitter()
{
    messagingSettings_t inSettings = {
        .start = msg_pipe_startInStub,
        .connect = msg_pipe_connectInStub,
    };
    messagingSettings_t outSettings = {
        .start = msg_pipe_startOutStub,
        .connect = msg_pipe_connectOutStub,
    };
    
    msg_pipe_splitterCalled = 0;
    msg_pipe_chain_t chain = {
        .splitter = msg_pipe_splitter_no_message,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(inSettings),
        .out = msg_core_createMessagingClient(outSettings),
        .out_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(msg_pipe_splitter_no_message,ctx->out_chain->splitter);
}