/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <syscfg/syscfg.h>
#include "cosa_webconfig_api.h"
#include "webconfig_framework.h"
#include "base64.h"
#include "safec_lib_common.h"

void initMultiCompMaster(void);

int  get_base64_decodedbuffer(char *pString, char **buffer, int *size)
{
    struct timespec start,end,*startPtr,*endPtr;
    int decodeMsgSize = 0;
    char *decodeMsg = NULL;
    if (buffer == NULL || size == NULL || pString == NULL)
        return -1;

    startPtr = &start;
    endPtr = &end;


    getCurrentTime(startPtr);
    decodeMsgSize = b64_get_decoded_buffer_size(strlen(pString));

    decodeMsg = (char *) malloc(sizeof(char) * decodeMsgSize);

    if (!decodeMsg)
        return -1;

    *size = b64_decode( (const uint8_t*)pString, strlen(pString), (uint8_t *)decodeMsg );
    CcspTraceWarning(("base64 decoded data contains %d bytes\n",*size));

    getCurrentTime(endPtr);
    CcspTraceWarning(("Base64 decode Elapsed time : %ld ms\n", timeValDiff(startPtr, endPtr)));

    *buffer = decodeMsg;
    return 0;
}

msgpack_unpack_return get_msgpack_unpack_status(char *decodedbuf, int size)
{

    msgpack_zone mempool;
    msgpack_object deserialized;
    msgpack_unpack_return unpack_ret;

    if (decodedbuf == NULL || !size)
        return MSGPACK_UNPACK_NOMEM_ERROR;

    msgpack_zone_init(&mempool, 2048);
    unpack_ret = msgpack_unpack(decodedbuf, size, NULL, &mempool, &deserialized);

    switch(unpack_ret)
    {
    case MSGPACK_UNPACK_SUCCESS:
        CcspTraceWarning(("MSGPACK_UNPACK_SUCCESS :%d\n",unpack_ret));
        break;
    case MSGPACK_UNPACK_EXTRA_BYTES:
        CcspTraceWarning(("MSGPACK_UNPACK_EXTRA_BYTES :%d\n",unpack_ret));
        break;
    case MSGPACK_UNPACK_CONTINUE:
        CcspTraceWarning(("MSGPACK_UNPACK_CONTINUE :%d\n",unpack_ret));
        break;
    case MSGPACK_UNPACK_PARSE_ERROR:
        CcspTraceWarning(("MSGPACK_UNPACK_PARSE_ERROR :%d\n",unpack_ret));
        break;
    case MSGPACK_UNPACK_NOMEM_ERROR:
        CcspTraceWarning(("MSGPACK_UNPACK_NOMEM_ERROR :%d\n",unpack_ret));
        break;
    default:
        CcspTraceWarning(("Message Pack decode failed with error: %d\n", unpack_ret));
    }

    msgpack_zone_destroy(&mempool);
    //End of msgpack decoding

    return unpack_ret;
}
int CheckIfIpIsValid( char *ipAddress )
{

    CcspTraceInfo(("%s:IpAddressReceivedIs:%s\n",__FUNCTION__,ipAddress));

    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;

    if ( (inet_pton(AF_INET, ipAddress, &(sa.sin_addr)) == 1 ) || (inet_pton(AF_INET6, ipAddress, &(sa6.sin6_addr)) == 1 ))
    {
        return VALID_IP;
    }

    return INVALID_IP;
}

int CheckIfPortsAreValid( char *port, char *port_end_range )
{

    CcspTraceInfo(("%s:ExternalPortEndRangeReceivedIs:%s\n",__FUNCTION__,port));
    CcspTraceInfo(("%s:ExternalPortEndRangeReceivedIs:%s\n",__FUNCTION__,port_end_range));


    int iPort = atoi(port);

    int iPort_end_range = atoi(port_end_range);

    while (*port)
    {
        if (isdigit(*port++) == 0)
        {
            return INVALID_PORT ;
        }
    }

    while (*port_end_range)
    {
        if (isdigit(*port_end_range++) == 0)
        {
            return INVALID_PORT ;
        }
    }

    if ( iPort <= 0 || iPort > 65535 || iPort_end_range <=0 || iPort_end_range > 65535  || iPort > iPort_end_range)
    {
        return INVALID_PORT ;
    }


    return 0;
}
/* API to get the subdoc version */


uint32_t getBlobVersion(char* subdoc)
{

    char subdoc_ver[64] = {0}, buf[72] = {0};
    snprintf(buf,sizeof(buf),"%s_version",subdoc);
    if ( syscfg_get( NULL, buf, subdoc_ver, sizeof(subdoc_ver)) == 0 )
    {
        int version = atoi(subdoc_ver);
        //  uint32_t version = strtoul(subdoc_ver, NULL, 10) ;

        return (uint32_t)version;
    }
    return 0;
}

/* API to update the subdoc version */
int setBlobVersion(char* subdoc,uint32_t version)
{

    char subdoc_ver[32] = {0}, buf[72] = {0};
    snprintf(subdoc_ver,sizeof(subdoc_ver),"%u",version);
    snprintf(buf,sizeof(buf),"%s_version",subdoc);

    if (strcmp(subdoc,"hotspot") == 0 )
    {
        char cmd[256] = {0};
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"mv /tmp/.%s%s %s",subdoc,subdoc_ver,HOTSPOT_BLOB_FILE);
        CcspTraceInfo(("%s : cmd to move filename is %s\n",__FUNCTION__,cmd));

        system(cmd);

    }
    if(syscfg_set_commit(NULL,buf,subdoc_ver) != 0)
    {
        CcspTraceError(("syscfg_set failed\n"));
        return -1;
    }

    return 0;

}

/* API to register all the supported subdocs , versionGet and versionSet are callback functions to get and set the subdoc versions in db */

void webConfigFrameworkInit()
{
    char *sub_docs[SUBDOC_COUNT+1]= {"macbinding","lan",(char *) 0 };

    blobRegInfo *blobData;

    blobData = (blobRegInfo*) malloc(SUBDOC_COUNT * sizeof(blobRegInfo));

    int i;
    memset(blobData, 0, SUBDOC_COUNT * sizeof(blobRegInfo));

    blobRegInfo *blobDataPointer = blobData;


    for (i=0 ; i < SUBDOC_COUNT ; i++ )
    {
        strncpy( blobDataPointer->subdoc_name, sub_docs[i], sizeof(blobDataPointer->subdoc_name)-1);

        blobDataPointer++;
    }

    blobDataPointer = blobData ;

    getVersion versionGet = getBlobVersion;

    setVersion versionSet = setBlobVersion;

    register_sub_docs(blobData,SUBDOC_COUNT,versionGet,versionSet);
}

void getCurrentTime(struct timespec *timer)
{
    clock_gettime(CLOCK_REALTIME, timer);
}

long timeValDiff(struct timespec *starttime, struct timespec *finishtime)
{
    long msec;
    msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
    msec+=(finishtime->tv_nsec-starttime->tv_nsec)/1000000;
    return msec;
}
