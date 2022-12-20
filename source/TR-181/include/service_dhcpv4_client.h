/************************************************************************************
  If not stated otherwise in this file or this component's LICENSE file the
  following copyright and licenses apply:

  Copyright 2018 RDK Management

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
************************************************************************************/

#include <stdio.h>
#include "ccsp_trace.h"

typedef unsigned int token_t;

/*  ENUMERATION DECLARATIONS  */
typedef enum _wan_prot {
    PROT_DHCP,
    PROT_STATIC,
} wan_prot;

typedef enum _rt_mod {
    RTMOD_UNKNOW,
    RTMOD_IPV4, // COSA_DML_DEVICE_MODE_Ipv4 - 1
    RTMOD_IPV6, // COSA_DML_DEVICE_MODE_Ipv6 - 1
    RTMOD_DS,   // COSA_DML_DEVICE_MODE_Dualstack - 1
} rt_mod;


/*  STRUCTURE DECLARATIONS  */
typedef struct _serv_dhcp {
    char            ifname[16];
    int             sefd;
    token_t         setok;
    rt_mod          rtmod;
    wan_prot        prot;
} serv_dhcp;


/*  FUNCTION DECLARATIONS  */
int serv_dhcp_init();
int get_dhcpc_pidfile(char *pidfile,int size);
int dhcp_parse_vendor_info( char *options, const int length, char *ethWanMode);
int dhcpv4_client_service_start(serv_dhcp *sd);
int dhcpv4_client_service_stop(serv_dhcp *sd);
int dhcpv4_client_service_restart(serv_dhcp *sd);
int dhcpv4_client_service_renew(serv_dhcp *sd);
int dhcpv4_client_service_release(serv_dhcp *sd);
int dhcpv4_client_start(serv_dhcp *sd);
int dhcpv4_client_stop(const char *ifname);
