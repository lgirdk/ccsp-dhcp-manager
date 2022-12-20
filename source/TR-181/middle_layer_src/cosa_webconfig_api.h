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

#ifndef  _COSA_WEBCONFIG_API_H
#define  _COSA_WEBCONFIG_API_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "ansc_status.h"

#include "msgpack.h"
#include "webconfig_framework.h"

#define SUBDOC_COUNT 2

#define BLOCK_SIZE 32
#define VAL_BLOCK_SIZE 129 // for ipv6 address 128 + 1 size is needed

#define SinglePortForwardCount
#define ALIAS_PRE_SPF "spf_"
#define ALIAS_PRE_PFR "pfr_"
#define ALIAS_POS_PROTO "::protocol"
#define ALIAS_POS_NAME "::name"
#define ALIAS_POS_EXT_PORT "::external_port"
#define ALIAS_POS_IP "::to_ip"
#define ALIAS_POS_ENABLE "::enabled"
#define ALIAS_SPF "SinglePortForward_"

#define ALIAS_PFR "PortRangeForward_"
#define ALIAS_PFR_INT_RANGE "::internal_port_range_size"
#define ALIAS_PFR_PUBLIC_IP "::public_ip"
#define ALIAS_POS_EXT_PORT_RANGE "::external_port_range"

#define ALIAS_POS_IPV6 "::to_ipv6"
#define ALIAS_POS_INT_PORT "::internal_port"
#define ALIAS_POS_PREV_STATE "::prev_rule_enabled_state"

#define HOTSPOT_BLOB_FILE "/nvram/hotspot_blob"

#ifdef WEBCFG_TEST_SIM

#define NACK_SIMULATE_FILE "/tmp/sim_nack"
#define TIMEOUT_SIMULATE_FILE "/tmp/sim_timeout"

#endif

typedef struct {
    char cmd[BLOCK_SIZE];       
    char val[VAL_BLOCK_SIZE];
} t_cache;


uint32_t getBlobVersion(char* subdoc);
int setBlobVersion(char* subdoc,uint32_t version);
void webConfigFrameworkInit() ;

int  get_base64_decodedbuffer(char *pString, char **buffer, int *size);
msgpack_unpack_return get_msgpack_unpack_status(char *decodedbuf, int size);
void getCurrentTime(struct timespec *timer);
long timeValDiff(struct timespec *starttime, struct timespec *finishtime);

#endif
