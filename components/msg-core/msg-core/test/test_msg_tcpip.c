#include "unity.h"
#include "msg_core.h"
#include "msg_tcpip.h"
#include "logger.h"
static int msg_tcpip_dummyReadNoMessageCalled = 0;
static int msg_tcpip_dummyReadCalled = 0;
static int msg_tcpip_dummyWriteCalled = 0;
static int msg_tcpip_dummyConnectCalled = 0;

int msg_tcpip_dummyRead_noMessage(int socket,uint8_t* buf, size_t size)
{
    msg_tcpip_dummyReadNoMessageCalled ++;
    return 0;
}
int msg_tcpip_dummyRead(int socket,uint8_t* buf, size_t size)
{
    //uint8_t * readBuffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE);
    msg_tcpip_dummyReadCalled ++;
    return size;
}

int msg_tcpip_dummyWrite(int socket,uint8_t* buf, size_t size)
{
    msg_tcpip_dummyWriteCalled ++;
    return size;
}

int msg_tcpip_dummyConnect(messagingClient_t *client)
{
    msg_tcpip_dummyConnectCalled ++;
    return 1;
}

void test_msg_tcpip_create_tcpip_client(void)
{
    tcpIpSettings_t settings;
    
    messagingClient_t * client = msg_tcpip_createTcpIpClient(settings);

    TEST_ASSERT_NOT_NULL(client);
} 
void test_msg_tcpip_client_connect(void)
{
    tcpIpSettings_t settings = {
        .connect = msg_tcpip_dummyConnect,
    };
    
    messagingClient_t * client = msg_tcpip_createTcpIpClient(settings);

    TEST_ASSERT_EQUAL(0,msg_tcpip_connect(client));

    TEST_ASSERT_EQUAL(1,client->connected);
}  
void test_msg_tcpip_client_outgoing_handler(void)
{
    uint8_t buffer[4];
    
    tcpip_ctx_t ctx = {
        .socket = 1,
        .readBuffer = &buffer,
        .write = msg_tcpip_dummyWrite,
        .read = msg_tcpip_dummyRead 
    };
    messagingClient_t client;
    client.ctx = (void *) &ctx;
    client.connected = 1;
    message_t message;
    uint8_t data[] = {1,2,3,4}; 
    message.data = &data;
    message.length = 4;
    msg_tcpip_outgoingHandler(&client,&message);
    TEST_ASSERT_EQUAL(1,msg_tcpip_dummyWriteCalled);
} 
void test_msg_tcpip_client_incoming_handler_no_message(void)
{
    tcpip_ctx_t ctx = {
        .socket = 1,
        .readBuffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE),
        .write = msg_tcpip_dummyWrite,
        .read = msg_tcpip_dummyRead_noMessage,
    };
    
    messagingClient_t client = {
        .ctx = &ctx,
        .connected = 1,
    };
    message_t *message = msg_tcpip_incomingHandler(&client);
    TEST_ASSERT_NULL(message);
    TEST_ASSERT_EQUAL(1,msg_tcpip_dummyReadNoMessageCalled);

} 
void test_msg_tcpip_client_incoming_handler(void)
{
    tcpip_ctx_t ctx = {
        .socket = 1,
        .readBuffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE),
        .write = msg_tcpip_dummyWrite,
        .read = msg_tcpip_dummyRead
    };
    messagingClient_t client = {
        .ctx = &ctx,
        .connected = 1,
    };
    client.ctx = (void *) &ctx;
    message_t *message = msg_tcpip_incomingHandler(&client);
    TEST_ASSERT_EQUAL(1,msg_tcpip_dummyReadCalled);
} 