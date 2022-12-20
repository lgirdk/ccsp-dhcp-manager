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

#if defined (_XB6_PRODUCT_REQ_) || defined(_CBR_PRODUCT_REQ_) || defined (_XB7_PRODUCT_REQ_)
#define CONSOLE_LOG_FILE "/rdklogs/logs/Consolelog.txt.0"
#else
#define CONSOLE_LOG_FILE "/rdklogs/logs/ArmConsolelog.txt.0"
#endif

extern void executeCmd(char *);

static async_id_t l_sAsyncID[7];
static pthread_t sysevent_tid_v6s;
static int sysevent_fd_v6s;
static token_t sysevent_token_v6s;

#define BUFF_LEN_8      8
#define BUFF_LEN_16    16
#define BUFF_LEN_32    32
#define BUFF_LEN_64    64
#define BUFF_LEN_128  128
#define BUFF_LEN_256  256

static void *dhcpv4_server_sysevent_handler();

int dhcp_server_init()
{
	char l_cDhcp_Server_Enabled[BUFF_LEN_8] = {0};

	syscfg_get(NULL, "dhcp_server_enabled", l_cDhcp_Server_Enabled, sizeof(l_cDhcp_Server_Enabled));

	if (!strncmp(l_cDhcp_Server_Enabled, "1", 1))
        {
             pthread_create(&sysevent_tid_v6s, NULL, dhcpv4_server_sysevent_handler, NULL);
             CcspTraceInfo(("%s Creating dhcpv4_server_sysevent_handler monitor thread\n", __func__));
        }
        return 0;
}

static void *dhcpv4_server_sysevent_handler()
{
    CcspTraceInfo(("Entering %s thread\n", __func__));

    sysevent_fd_v6s = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "dhcpv6_server_handler", &sysevent_token_v6s);

    sysevent_set_options(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_server-start", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_server-start", &l_sAsyncID[0]);

    sysevent_set_options(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_server-stop", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_server-stop", &l_sAsyncID[1]);

    sysevent_set_options(sysevent_fd_v6s, sysevent_token_v6s, "lan-status", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_v6s, sysevent_token_v6s, "lan-status", &l_sAsyncID[2]);

    sysevent_set_options(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_server-restart", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_server-restart", &l_sAsyncID[3]);

    sysevent_set_options(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_server-resync", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_server-resync", &l_sAsyncID[4]);

    sysevent_set_options(sysevent_fd_v6s, sysevent_token_v6s, "syslog-status", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_v6s, sysevent_token_v6s, "syslog-status", &l_sAsyncID[5]);

    sysevent_set_options(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_conf_change", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_v6s, sysevent_token_v6s, "dhcp_conf_change", &l_sAsyncID[6]);



    for (;;)
    {
       char name[BUFF_LEN_128], val[BUFF_LEN_128];
       memset(name,0,sizeof(name));
       memset(val,0,sizeof(val));
       int namelen = sizeof(name);
       int vallen  = sizeof(val);
       int err;
       async_id_t getnotification_id_v6s;

       err = sysevent_getnotification(sysevent_fd_v6s, sysevent_token_v6s, name, &namelen,
                                      val, &vallen, &getnotification_id_v6s);

       if (err)
       {
            CcspTraceError(("sysevent_getnotification failed with error: %d %s\n", err,__FUNCTION__));
            CcspTraceError(("sysevent_getnotification failed name: %s val : %s\n", name,val));
            if ( 0 != v_secure_system("pidof syseventd")) {
                CcspTraceWarning(("%s syseventd not running ,breaking the receive notification loop \n",__FUNCTION__));
                break;
            }

       }
       else
       {
            //Hardcoded dhcp_server_start call for now, Further modifications will be taken care later
            if ((!strncmp(name,"dhcp_server-start",17)) || (!strncmp(name,"dhcp_server-restart",19)))
            {
                 if ((access("/var/tmp/lan_not_restart", F_OK)) == -1)
                 {
             	     dhcp_server_start("lan-status");
                 }
	         else
	         {
		     dhcp_server_start("lan_not_restart");
	         }
		 CcspTraceInfo(("%s Calling dhcp_server_start\n", __func__));
            }
            else if (!strncmp(name,"dhcp_server-stop",16))
            {
                 dhcp_server_stop();
                 CcspTraceInfo(("%s  Calling dhcp_server-stop\n", __func__));
            }
            //Hardcoded lan_status_change call for now, Further modifications will be taken care later
            else if (!strncmp(name,"lan-status",10))
            {
                 if ((access("/var/tmp/lan_not_restart", F_OK)) == -1)
                 {
                    lan_status_change("lan-status");
                 }
                 else
                 {
                    lan_status_change("lan_not_restart");
                 }
                 CcspTraceInfo(("%s Calling lan_status_change\n", __func__));
            }
            else if(!strncmp(name, "syslog-status", 13))
            {
                 char syslog_status_buf[10]={0};
                 sysevent_get(sysevent_fd_v6s, sysevent_token_v6s,
                     "syslog-status", syslog_status_buf,
                     sizeof(syslog_status_buf));
                 if(!strncmp(syslog_status_buf, "started", 7))
                 {
                     syslog_restart_request();
                 }
            }
            else if(!strncmp(name, "dhcp_server-resync", 18))
            {
                 resync_to_nonvol(NULL);
            }
            #ifdef RDKB_EXTENDER_ENABLED
            else if(!strncmp(argv[1], "dhcp_conf_change", 16))
            {
           	UpdateDhcpConfChangeBasedOnEvent();
	        dhcp_server_start(NULL);
            }
            #endif
       }

    }
    CcspTraceInfo(("Exiting %s thread\n", __func__));
    return NULL;
}

