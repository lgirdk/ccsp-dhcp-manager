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
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>
#include "syscfg/syscfg.h"
#include <sys/stat.h>
#include "platform_hal.h"

#define TRUE_STR               "true"
#define TRUE                   1
#define FALSE                  0
#define SUCCESS                0
#define FAILURE                1
#define BUFLEN_4               4           //!< buffer length 4
#define BUFLEN_8               8           //!< buffer length 8
#define BUFLEN_16              16          //!< buffer length 16
#define BUFLEN_18              18          //!< buffer length 18
#define BUFLEN_24              24          //!< buffer length 24
#define BUFLEN_32              32          //!< buffer length 32
#define BUFLEN_40              40          //!< buffer length 40
#define BUFLEN_48              48          //!< buffer length 48
#define BUFLEN_64              64          //!< buffer length 64
#define BUFLEN_80              80          //!< buffer length 80
#define BUFLEN_128             128         //!< buffer length 128
#define BUFLEN_256             256         //!< buffer length 256
#define BUFLEN_264             264         //!< buffer length 264
#define BUFLEN_512             512         //!< buffer length 512
#define BUFLEN_1024            1024        //!< buffer length 1024
#define CONSOLE_LOG_FILE       "/rdklogs/logs/DHCPMGRLog.txt.0"

#define COLLECT_WAIT_INTERVAL_MS          4
#define USECS_IN_MSEC                     1000
#define MSECS_IN_SEC                      1000
#define RETURN_PID_TIMEOUT_IN_MSEC        (5 * MSECS_IN_SEC)    // 5 sec
#define RETURN_PID_INTERVAL_IN_MSEC       (0.5 * MSECS_IN_SEC)  // 0.5 sec - half a second

//DHCPv6 Options
#define DHCPV6_OPT_82  82  // OPTION_SOL_MAX_RT: Solicite Maximum Retry Time
#define DHCPV6_OPT_23  23  // OPTION_SOL_MAX_RT: Solicite Maximum Retry Time
#define DHCPV6_OPT_95  95  // OPTION_SOL_MAX_RT: Solicite Maximum Retry Time
#define DHCPV6_OPT_24  24  // OPTION_DOMAIN_LIST
#define DHCPV6_OPT_83  83  // OPTION_INF_MAX_RT
#define DHCPV6_OPT_17  17  // OPTION_VENDOR_OPTS
#define DHCPV6_OPT_31  31  // OPTION_SNTP_SERVERS
#define DHCPV6_OPT_15  15  // User Class Option
#define DHCPV6_OPT_16  16  // Vendor Class Option
#define DHCPV6_OPT_20  20  // Reconfigure Accept Option


//DHCPv4 Options
#define DHCPV4_OPT_42  42  // NTP Server Addresses
#define DHCPV4_OPT_43  43  // Vendor Specific Information
#define DHCPV4_OPT_58  58  // DHCP Renewal (T1) Time
#define DHCPV4_OPT_59  59  // DHCP Rebinding (T2) Time
#define DHCPV4_OPT_60  60  // Class Identifier
#define DHCPV4_OPT_61  61  // Client Identifier
#define DHCPV4_OPT_100 100 // IEEE 1003.1 TZ String
#define DHCPV4_OPT_122 122 // CableLabs Client Configuration
#define DHCPV4_OPT_125 125 // Vendor-Identifying Vendor-Specific Information
#define DHCPV4_OPT_242 242 // Private Use
#define DHCPV4_OPT_243 243 // Private Use
#define DHCPV4_OPT_END 255 // DHCP Option End - used to check if option is valid
#define DHCPV4_OPT_120 120 //DHCP Req option for sipsrv
#define DHCPV4_OPT_121 121 //DHCP Req option for classless static routes

#define DBG_PRINT(fmt ...)     {\
    FILE     *fp        = NULL;\
    fp = fopen ( CONSOLE_LOG_FILE, "a+");\
    if (fp)\
    {\
        fprintf(fp,fmt);\
        fclose(fp);\
    }\
}\

#define UNUSED_VARIABLE(x) (void)(x)

typedef enum {
    WAN_LOCAL_IFACE = 1,
    WAN_REMOTE_IFACE,
} IfaceType;

typedef struct dhcp_opt {
    char * ifname;
    IfaceType ifType;
} dhcp_params;

pid_t start_dhcpv4_client (dhcp_params * params,dhcp_opt_list * req_opt_list,dhcp_opt_list * send_opt_list);
int stop_dhcpv4_client (dhcp_params * params);
int stop_udhcpc (dhcp_params * params);
pid_t start_dhcpv6_client (dhcp_params * params);
int stop_dhcpv6_client (dhcp_params * params);
pid_t start_exe(char * exe, char * args);
pid_t return_dhcp6_client_pid ();
pid_t get_process_pid (char * name, char * args);
int collect_waiting_process(int pid, int timeout);
void free_opt_list_data (dhcp_opt_list * opt_list);
int signal_process (pid_t pid, int signal);
int add_dhcpv4_opt_to_list (dhcp_opt_list ** opt_list, int opt, char * opt_val);
