#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include "msg_core.h"


#define LOG_NONE 0
#define LOG_DEBUG 4

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_NONE
#endif
//#define LOGGING_OFF

//void msgBundleDump(const char * tag, messageBundle_t * bundle);

#ifndef __XTENSA__

static void hexdump(const char * tag, const unsigned char * buffer, const int length, const int level);

#if LOG_LEVEL != LOG_NONE
#define LOG printf
#else
static void nop() {}
#define LOG nop
#endif
#define LOG_I(TAG, FORMAT , ...) LOG("INFO - %s: " FORMAT "\n", TAG, ##__VA_ARGS__) 
#define LOG_V(TAG, FORMAT , ...) LOG ("VERBOSE - %s: " FORMAT "\n", TAG, ##__VA_ARGS__)
#define LOG_D(TAG, FORMAT , ...) LOG ("DEBUG - %s: " FORMAT "\n", TAG, ##__VA_ARGS__)
#define LOG_E(TAG, FORMAT , ...) LOG ("ERROR - %s: " FORMAT "\n", TAG, ##__VA_ARGS__)
#define LOG_W(TAG, FORMAT , ...) LOG ("WARNING - %s: " FORMAT "\n", TAG, ##__VA_ARGS__)
#define LOG_BUFFER_HEXDUMP(TAG, BUFFER, LENGTH, LEVEL) hexdump(TAG, BUFFER, LENGTH,LEVEL)
#define LOG_MSG_BUNDLE(TAG, BUNDLE) LOG_W(TAG, "Message bundle not Implemented")

//#define LOG_MSG_BUNDLE(TAG, BUNDLE) msgBundleDump(TAG, BUNDLE)
#else
#include "esp_log.h"

#define LOG_DEBUG ESP_LOG_DEBUG
#define LOG_I(...) ESP_LOGI(__VA_ARGS__)
#define LOG_V(...) ESP_LOGV(__VA_ARGS__)
#define LOG_D(...) ESP_LOGD(__VA_ARGS__)
#define LOG_E(...) ESP_LOGE(__VA_ARGS__)
#define LOG_W(...) ESP_LOGW(__VA_ARGS__)
#define LOG_BUFFER_HEXDUMP(...) ESP_LOG_BUFFER_HEXDUMP(__VA_ARGS__)
//#define LOG_MSG_BUNDLE(TAG, BUNDLE) msgBundleDump(TAG, BUNDLE)
#define LOG_MSG_BUNDLE(TAG, BUNDLE) printf("Message bundle not Implemented\n")

#endif

static void hexdump(const char * tag, const unsigned char * buffer, const int length, const int level)
{
#if LOG_LEVEL != LOG_NONE
    if(length <= 0 || buffer == NULL) return;

    char out[17];
    memset(&out,'\0',17);
        
    printf("%s: ",tag);
    int i = 0;
    for(i=0;i<length;i++)
    {
        printf("%02x ",buffer[i]);
        out[i % 16] = (isprint(buffer[i]) ? buffer[i] : '.');
        if((i+1) % 8 == 0) printf(" ");
        if((i+1) % 16 ==0) {
            out[16] = '\0';
            printf(" | %s |\n%s: ",out,tag);
        }
    }
    if((i % 16) + 1 != 0)
    {
        int num = (16 - (i % 16)) * 3;
        num = ((i % 16) < 8 ? num + 1 : num);
        out[(i % 16)] = '\0';
        char padding[(16 * 3) + 2];
        memset(&padding,' ',num+1);
        padding[(16-i)*3] = '\0';
        printf("%s | %s |\n",padding,out);
    }
    printf("\n");
#endif
}/*
void msgBundleDump(const char * tag, messageBundle_t * bundle)
{
    for(int i = 0;i<bundle->numMessages;i++)
    {
        LOG_D(tag,"Message %d",i);
        LOG_BUFFER_HEXDUMP(tag,bundle->messages[i]->data,bundle->messages[i]->length,LOG_DEBUG);            
    }
} */
#endif
