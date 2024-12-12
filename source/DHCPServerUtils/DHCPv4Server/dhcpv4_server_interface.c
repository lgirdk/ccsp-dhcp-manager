/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <net/if.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <secure_wrapper.h>
#include <safec_lib_common.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "sysevent/sysevent.h"
#include "syscfg/syscfg.h"
#include "dhcp_server_functions.h"
#include "service_dhcp_server.h"
#include "dhcpv4_server_interface.h"
#include "errno.h"
#include "util.h"
#include "ifl.h"

#define DHCP_SERVER_START   "dhcp_server-start"
#define DHCP_SERVER_STOP    "dhcp_server-stop"
#define DHCP_SERVER_RESTART "dhcp_server-restart"
#define DHCP_SERVER_RESYNC  "dhcp_server-resync"
#define LAN_STATUS          "lan-status"
#define SYSLOG_STATUS       "syslog-status"
#define DHCP_CONF_CHANGE    "dhcp_conf_change"

#define DHCPV4S_CALLER_CTX  "dhcpv4_server"

extern void executeCmd(char *);

#define BUFF_LEN_8      8
#define BUFF_LEN_16    16
#define BUFF_LEN_32    32
#define BUFF_LEN_64    64
#define BUFF_LEN_128  128
#define BUFF_LEN_256  256

int dhcp_server_init()
{
    DHCPMGR_LOG_INFO("DHCPV4 server event registration started\n");
    char l_cDhcp_Server_Enabled[BUFF_LEN_8] = {0};

    syscfg_get(NULL, "dhcp_server_enabled", l_cDhcp_Server_Enabled, sizeof(l_cDhcp_Server_Enabled));

    if (!strncmp(l_cDhcp_Server_Enabled, "1", 1))
    {
        if (IFL_SUCCESS != ifl_init_ctx(DHCPV4S_CALLER_CTX, IFL_CTX_DYNAMIC))
        {
            DHCPMGR_LOG_INFO("Failed to init ifl ctx for %s", DHCPV4S_CALLER_CTX);
        }

        //Hardcoded dhcp_server_start call for now, Further modifications will be taken care later
        ifl_register_event_handler(DHCP_SERVER_START, IFL_EVENT_NOTIFY_TRUE, DHCPV4S_CALLER_CTX, dhcp_server_start);
        ifl_register_event_handler(DHCP_SERVER_RESTART, IFL_EVENT_NOTIFY_TRUE, DHCPV4S_CALLER_CTX, dhcp_server_restart);
        ifl_register_event_handler(DHCP_SERVER_STOP, IFL_EVENT_NOTIFY_TRUE, DHCPV4S_CALLER_CTX, dhcp_server_stop);
        ifl_register_event_handler(LAN_STATUS, IFL_EVENT_NOTIFY_FALSE, DHCPV4S_CALLER_CTX, lan_status_change);
        ifl_register_event_handler(SYSLOG_STATUS, IFL_EVENT_NOTIFY_FALSE, DHCPV4S_CALLER_CTX, syslog_restart_request);
        ifl_register_event_handler(DHCP_SERVER_RESYNC, IFL_EVENT_NOTIFY_TRUE, DHCPV4S_CALLER_CTX, resync_to_nonvol);

    #ifdef RDKB_EXTENDER_ENABLED
        ifl_register_event_handler(DHCP_CONF_CHANGE, IFL_EVENT_NOTIFY_FALSE, DHCPV4S_CALLER_CTX, dhcp_server_start);
    #endif

    }
    DHCPMGR_LOG_INFO("DHCPV4 server event registration completed\n");
    return 0;
}
