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

#include <stdbool.h>
#define INET6_ADDRSTRLEN    46
/*erouter topology mode*/
enum tp_mod {
    TPMOD_UNKNOWN,
    FAVOR_DEPTH,
    FAVOR_WIDTH,
};

struct serv_ipv6 {
    int         sefd;
    int         setok;

    bool        wan_ready;

    char        mso_prefix[INET6_ADDRSTRLEN];
    enum tp_mod tpmod;
};


void dhcp_server_stop();
int dhcp_server_start(char *);
//void dhcp_server_restart();
int service_dhcp_init();
void lan_status_change(char *);
int init_dhcp_server_service();
int dhcpv6s_start(struct serv_ipv6 *si6);
int dhcpv6s_stop(struct serv_ipv6 *si6);
int dhcpv6s_restart(struct serv_ipv6 *si6);
int serv_ipv6_init(struct serv_ipv6 *si6);
int serv_ipv6_term(struct serv_ipv6 *si6);
int return_dibbler_server_pid ();
int syslog_restart_request();
void resync_to_nonvol(char *);

#define DHCPMGR_LOG_INFO(format, ...)     \
                              CcspTraceInfo   (("%s - "format"\n", __FUNCTION__, ##__VA_ARGS__))
#define DHCPMGR_LOG_ERROR(format, ...)    \
                              CcspTraceError  (("%s - "format"\n", __FUNCTION__, ##__VA_ARGS__))
#define DHCPMGR_LOG_NOTICE(format, ...)   \
                              CcspTraceNotice (("%s - "format"\n", __FUNCTION__, ##__VA_ARGS__))
#define DHCPMGR_LOG_WARNING(format, ...)  \
                              CcspTraceWarning(("%s - "format"\n", __FUNCTION__, ##__VA_ARGS__))
