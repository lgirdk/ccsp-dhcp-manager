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

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/

#ifndef __SERV_UTIL__
#define __SERV_UTIL__
#include <sysevent/sysevent.h>

#define SE_SERV         "127.0.0.1"

#ifndef NELEMS
#define NELEMS(arr)     (sizeof(arr) / sizeof((arr)[0]))
#endif

int vsystem(const char *fmt, ...);
int iface_get_hwaddr(const char *ifname, char *mac, size_t size);
int iface_get_ipv4addr(const char *ifname, char *ipv4Addr, size_t size);
int is_iface_present(const char *ifname);

int serv_can_start(int sefd, token_t tok, const char *servname);
int serv_can_stop(int sefd, token_t tok, const char *servname);
int pid_of(const char *name, const char *keyword);

#define DHCPMGR_LOG_INFO(format, ...)     \
                              CcspTraceInfo   (("%s - "format"\n", (char *)__FUNCTION__, ##__VA_ARGS__))
#define DHCPMGR_LOG_ERROR(format, ...)    \
                              CcspTraceError  (("%s - "format"\n", (char *)__FUNCTION__, ##__VA_ARGS__))
#define DHCPMGR_LOG_NOTICE(format, ...)   \
                              CcspTraceNotice (("%s - "format"\n", (char *)__FUNCTION__, ##__VA_ARGS__))
#define DHCPMGR_LOG_WARNING(format, ...)  \
                              CcspTraceWarning(("%s - "format"\n", (char *)__FUNCTION__, ##__VA_ARGS__))

#endif /* __SW_UTIL__ */
