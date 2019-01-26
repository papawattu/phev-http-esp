#include "unity.h"
#include "msg_pipe_splitter.h"
#include "msg_core.h"
#include "msg_pipe.h"
#include "logger.h"
#include "msg_utils.h"

static int msg_pipe_splitter_splitterSingleCalled = 0;
message_t * msg_pipe_splitter_single_message(message_t * message)
{
    msg_pipe_splitter_splitterSingleCalled ++;
    return NULL;
}

static int msg_pipe_splitter_mock_single_splitter_called = 0;

messageBundle_t * msg_pipe_splitter_mock_single_splitter(void * ctx, message_t *message) 
{
    messageBundle_t * out = malloc(sizeof(messageBundle_t));

    out->messages[0] = msg_utils_copyMsg(message);

    out->numMessages = 1;

    msg_pipe_splitter_mock_single_splitter_called ++;
    
    return out;
} 
void test_msg_pipe_splitter_should_split_single()
{
    msg_pipe_ctx_t * ctx = NULL;
    msg_pipe_chain_t chain = {
        .splitter = msg_pipe_splitter_mock_single_splitter,
    };

    const uint8_t data[] = {1,2,3,4};

    message_t * message = msg_utils_createMsg(&data,sizeof(data));
    
    messageBundle_t * out = msg_pipe_splitter(ctx, &chain, message);
    
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(1,out->numMessages);
    TEST_ASSERT_NOT_NULL(out->messages);
    TEST_ASSERT_NOT_NULL(out->messages[0]);
    TEST_ASSERT_EQUAL(1,msg_pipe_splitter_mock_single_splitter_called);
    TEST_ASSERT_NOT_NULL(out->messages);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data,out->messages[0]->data,4);
}
static int msg_pipe_splitter_mock_double_splitter_called = 0;

messageBundle_t * msg_pipe_splitter_mock_double_splitter(void * ctx, message_t *message) 
{
    const uint8_t first_data[] = {1,2,3,4};
    const uint8_t second_data[] = {5,6,7,8};

    message_t * first_message = msg_utils_createMsg(&first_data,sizeof(first_data));
    
    message_t * second_message = msg_utils_createMsg(&second_data,sizeof(second_data));
    
    messageBundle_t * out = malloc(sizeof(messageBundle_t));

    out->messages[0] = first_message;
    out->messages[1] = second_message;

    out->numMessages = 2;

    msg_pipe_splitter_mock_double_splitter_called ++;
    
    return out;
} 
void test_msg_pipe_splitter_should_split_double()
{
    msg_pipe_ctx_t * ctx = NULL;
    msg_pipe_chain_t chain = {
        .splitter = msg_pipe_splitter_mock_double_splitter,
    };

    const uint8_t data[] = {1,2,3,4,5,6,7,8};
    const uint8_t first_data[] = {1,2,3,4};
    const uint8_t second_data[] = {5,6,7,8};


    message_t * message = msg_utils_createMsg(&data,sizeof(data));
    
    messageBundle_t * out = msg_pipe_splitter(ctx, &chain, message);
    
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(2,out->numMessages);
    TEST_ASSERT_NOT_NULL(out->messages);
    TEST_ASSERT_NOT_NULL(out->messages[0]);
    TEST_ASSERT_NOT_NULL(out->messages[1]);
    TEST_ASSERT_EQUAL(1,msg_pipe_splitter_mock_double_splitter_called);
    TEST_ASSERT_NOT_NULL(out->messages);
    TEST_ASSERT_EQUAL(sizeof(first_data),out->messages[0]->length);
    TEST_ASSERT_EQUAL(sizeof(second_data),out->messages[1]->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(first_data,out->messages[0]->data,sizeof(first_data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(second_data,out->messages[1]->data,sizeof(second_data));

} 
void test_msg_pipe_splitter_should_aggregate_message_bundle()
{
    messageBundle_t * messages = malloc(sizeof(messageBundle_t));
    
    const uint8_t data[] = {1,2,3,4,5,6,7,8};
    const uint8_t first_data[] = {1,2,3,4};
    const uint8_t second_data[] = {5,6,7,8};

    message_t * first_message = msg_utils_createMsg(&first_data,sizeof(first_data));
    
    message_t * second_message = msg_utils_createMsg(&second_data,sizeof(second_data));
    
    messages->messages[0] = first_message;
    messages->messages[1] = second_message;
    messages->numMessages = 2;
    
    message_t * out = msg_pipe_splitter_aggregrator(messages);

    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(8,out->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data,out->data,sizeof(data));
    
} 