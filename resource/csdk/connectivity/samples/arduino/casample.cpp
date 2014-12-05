/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <ctype.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cacommon.h"
#include "cainterface.h"
#include <TimedAction.h>
#include "Arduino.h"

#include "oic_malloc.h"

#define MAX_BUF_LEN 100 //1024
#define MAX_OPT_LEN 100

#define printf Serial.println
//#define printf

void print_menu();
void process();

void initialize();
void start_listening_server();
void start_discovery_server();
void find_resource();
void send_request();
void send_response(CARemoteEndpoint_t *endpoint, CAToken_t request_token);
void advertise_resource();
void send_notification();
void select_network();
void unselect_network();
void handle_request_response();

void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo);
void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo);
void send_request_tmp(CARemoteEndpoint_t *endpoint, CAToken_t token);
void terminate();

void getData(char *readInput, int bufferLength, int *dataLength)
{
    while (!Serial.available());
    int len = 0;
    while (Serial.available())
    {
        delay(100);
        char c = Serial.read();
        if ('\n' != c && '\r' != c && len < bufferLength - 1)
        {
            readInput[len++] = c;
        }
        else
        {
            break;
        }
    }

    readInput[len] = '\0';
    Serial.flush();
    Serial.print("PD:");
    Serial.println(readInput);
    (*dataLength) = len;
}

CAConnectivityType_t getConnectivityType()
{
    char type[2];
    memset(type, 0, sizeof(char));
    printf("Select network");
    printf("ETHERNET: 0");
    printf("WIFI: 1");
    printf("EDR: 2");
    printf("LE: 3");


    int16_t typeLen = 0;
    getData(type, sizeof(type), &typeLen);
    int num = type[0] - '0';
    switch (num)
    {
        case 0:
            return CA_ETHERNET;
        case 1:
            return CA_WIFI;
        case 2:
            return CA_EDR;
        case 3:
            return CA_LE;
    }
}
void setup()
{
    Serial.begin (115200);

    printf("============");
    printf("CA SAMPLE");
    printf("============");
    print_menu();
}

void loop()
{
    char buffer[5];
    memset(buffer, 0, sizeof(buffer));
    int16_t len;
    if (Serial.available() > 0)
    {
        getData(buffer, sizeof(buffer), &len);
        switch (toupper(buffer[0]))
        {
            case 'M': // menu
                print_menu();
                break;

            case 'Q': // quit
                printf("quit");
                return;

            case 'I': // Initialize interface
                initialize();
                break;

            case 'S': // start listening server
                start_listening_server();
                break;

            case 'D': // start discovery server
                start_discovery_server();
                break;

            case 'F': // find resource
                find_resource();
                break;

            case 'R': // send request
                send_request();
                break;

            case 'A': // advertise resource
                advertise_resource();
                break;

            case 'B': // send notification
                send_notification();
                break;

            case 'N': // select network
                select_network();
                break;

            case 'X': // unselect network
                unselect_network();
                break;

            case 'H': // handle request response
                handle_request_response();
                break;

            case 'T': // handle request response
                terminate();
                break;

            default:
                printf("wrong menu");
                break;
        }
    }
}

void initialize()
{
    CAInitialize();
    select_network();
    // set handler.
    CARegisterHandler(request_handler, response_handler);
}

void start_listening_server()
{
    printf("start server");
    CAStartListeningServer();
}

void start_discovery_server()
{
    printf("start server");
    CAStartDiscoveryServer();
}

void find_resource()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("ex) a/light");
    printf("uri: ");
    int len = 0;
    getData(buf, sizeof(buf), &len);

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!");
        token = NULL;
    }

    printf("token");
    printf((token != NULL) ? token : "");

    res = CAFindResource(buf, token);
    if (res != CA_STATUS_OK)
    {
        printf("find error");
    }
    else
    {
        printf("success: ");
        printf(buf);
    }
    CADestroyToken(token);
}

void send_request()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("10.11.12.13:4545/res_uri (for IP)");
    printf("10:11:12:13:45:45/res_uri (for BT)");
    printf("uri: ");

    int16_t len = 0;
    getData(buf, sizeof(buf), &len);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(buf, &endpoint);
    if (res != CA_STATUS_OK)
    {
        printf("Out of memory");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }
    endpoint->connectivityType = getConnectivityType();
    // create token
    CAToken_t token = NULL;
    res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token error");
        token = NULL;
    }

    printf((token != NULL) ? token : "");

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = "Json Payload";

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    CASendRequest(endpoint, &requestInfo);
    if (token != NULL)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyRemoteEndpoint(endpoint);
    }

    printf("============");
}

void advertise_resource()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("uri: ");

    int16_t len = 0;
    getData(buf, sizeof(buf), &len);

    int16_t optionNum = 0;
    char optionData[MAX_OPT_LEN];
    char optionNumBuf[2];

    printf("Option Num: ");
    getData(optionNumBuf, sizeof(optionNumBuf), &len);
    optionNum = optionNumBuf[0]  - '0';
    printf(optionNum);


    CAHeaderOption_t *headerOpt = NULL;
    if (optionNum > 0)
    {
        headerOpt = (CAHeaderOption_t *) OICMalloc(sizeof(CAHeaderOption_t) * optionNum);
        if (NULL == headerOpt)
        {
            printf("Out of memory");
            return;
        }
        memset(headerOpt, 0, sizeof(CAHeaderOption_t) * optionNum);
    }

    int i;
    for (i = 0 ; i < optionNum ; i++)
    {
        int optionID = 0;
        char getOptionID[2];
        getData(getOptionID, sizeof(getOptionID), &len);
        printf("Option Num: ");
        printf(i + 1);
        optionID = getOptionID[0];
        printf(optionID);

        headerOpt[i].optionID = optionID;
        memset(optionData, 0, sizeof(char) * MAX_OPT_LEN);
        printf("Option Data itr");
        printf(i + 1);

        int len = 0;
        getData(optionData, sizeof(optionData), &len);
        memcpy(headerOpt[i].optionData, optionData, strlen(optionData));
        printf(i + 1);
        printf("data");
        printf(optionData);

        headerOpt[i].optionLength = (uint16_t)strlen(optionData);
    }

    printf("============");
    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!");
        token = NULL;
    }

    printf("token");
    printf((token != NULL) ? token : "");

    CAAdvertiseResource(buf, token, headerOpt, (uint8_t)optionNum);
    if (NULL != headerOpt)
    {
        OICFree(headerOpt);
    }
    CADestroyToken(token);
}

void send_notification()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("10.11.12.13:4545/res_uri (for IP)");
    printf("10:11:12:13:45:45/res_uri (for BT)");
    printf("uri: ");

    int len = 0;
    getData(buf, sizeof(buf), &len);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(buf, &endpoint);
    if (CA_STATUS_OK != res)
    {
        printf("Out of memory");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }
    endpoint->connectivityType = getConnectivityType();
    CAInfo_t respondeData;
    memset(&respondeData, 0, sizeof(CAInfo_t));
    respondeData.token = "token";
    respondeData.payload = "Notification Data";

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = CA_CONTENT;
    responseInfo.info = respondeData;

    // send request
    CASendNotification(endpoint, &responseInfo);
    // destroy remote endpoint
    if (NULL != endpoint)
    {
        CADestroyRemoteEndpoint(endpoint);
    }
    printf("============");
}

void select_network()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("Select network");
    printf("ETHERNET: 0");
    printf("WIFI: 1");
    printf("EDR: 2");
    printf("LE: 3\n");

    int len = 0;
    getData(buf, sizeof(buf), &len);
    int number = buf[0] - '0';
    number = (number < 0 || number > 3) ? 1 : number;
    CASelectNetwork(1 << number);
    printf("============");
}

void unselect_network()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("Unselect network");
    printf("ETHERNET: 0");
    printf("WIFI: 1");
    printf("EDR: 2");
    printf("LE: 3\n");

    int len = 0;
    getData(buf, sizeof(buf), &len);
    int number = buf[0] - '0';
    printf(number);
    number = (number < 0 || number > 3) ? 1 : number;
    CAUnSelectNetwork(1 << number);
    printf("Terminate");
    CATerminate();
    printf("============");
}

void print_menu()
{

    printf("============");
    printf("i: Initialize");
    printf("s: start listening server");
    printf("d: start discovery server");
    printf("f: find resource");
    printf("r: send request");
    printf("a: advertise resource");
    printf("b: send notification");
    printf("n: select network");
    printf("x: unselect network");
    printf("h: handle request response");
    printf("t: terminate");
    printf("q: quit");
    printf("============");
}

void handle_request_response()
{
    CAHandleRequestResponse();
}

void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo)
{
    printf("uri: ");
    printf((object != NULL) ? object->resourceUri : "");
    printf("data: ");
    printf((requestInfo != NULL) ? requestInfo->info.payload : "");

    printf("send response");
    send_response((CARemoteEndpoint_t *)object, requestInfo->info.token);
}

void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo)
{
    printf("uri: ");
    printf((object != NULL) ? object->resourceUri : "");
    printf("data: ");
    printf((responseInfo != NULL) ? responseInfo->info.payload : "");
}

void send_response(CARemoteEndpoint_t *endpoint, CAToken_t request_token)
{

    printf("============");

    CAInfo_t responseData;
    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.token = request_token;
    responseData.payload = "response payload";

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = (CAResponseResult_t)203;
    responseInfo.info = responseData;

    // send request (connectivityType from remoteEndpoint of request Info)
    CASendResponse(endpoint, &responseInfo);

    printf("============");

}

void send_request_tmp(CARemoteEndpoint_t *endpoint, CAToken_t token)
{

    printf("============");

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = "Json Payload";

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    endpoint->connectivityType = CA_WIFI;
    CASendRequest(endpoint, &requestInfo);

    printf("============");

}

void terminate()
{
    unselect_network();
}