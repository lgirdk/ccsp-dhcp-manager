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

#include "dhcp_client_utils.h"
#include "udhcpc_client_utils.h"
#include "ifl.h"
#include <syscfg/syscfg.h>
#include <string.h>


#if DHCPV4_CLIEN_TI_UDHCPC
static pid_t start_ti_udhcpc (dhcp_params * params)
{
    if (params == NULL)
    {
        DBG_PRINT("%s %d: Invalid args..\n", __FUNCTION__, __LINE__);
        return FAILURE;
    }
}
#endif  // DHCPV4_CLIENT_TI_UDHCPC

/*
 * add_dhcpv4_opt_to_list ()
 * @description: util function to add DHCP opt and DHCP opt value to list
 * @params     : opt_list - output param to add DHCP options
               : opt - DHCP option
               : opt_val - DHCP option value - optional
 * @return     : returns the SUCCESS on adding option to list, else returns failure
 *
 */

int add_dhcpv4_opt_to_list (dhcp_opt_list ** opt_list, int opt, char * opt_val)
{

    if ((opt_list == NULL) || (opt <= 0) ||(opt >= DHCPV4_OPT_END) )
    {
        return RETURN_ERR;
    }

    dhcp_opt_list * new_dhcp_opt = malloc (sizeof(dhcp_opt_list));
    if (new_dhcp_opt == NULL)
    {
        return RETURN_ERR;
    }
    memset (new_dhcp_opt, 0, sizeof(dhcp_opt_list));
    new_dhcp_opt->dhcp_opt = opt;
    new_dhcp_opt->dhcp_opt_val = opt_val;

    if (*opt_list != NULL)
    {
        new_dhcp_opt->next = *opt_list;
    }
    *opt_list = new_dhcp_opt;

    return RETURN_OK;

}

#if 0
/*
 * get_dhcpv4_opt_list ()
 * @description: Returns a list of DHCP REQ and a list of DHCP SEND options
 * @params     : req_opt_list - output param to fill the DHCP REQ options
               : send_opt_list - output param to fill the DHCP SEND options
 * @return     : returns the SUCCESS on successful fetching of DHCP options, else returns failure
 *
 */

static int get_dhcpv4_opt_list (dhcp_opt_list ** req_opt_list, dhcp_opt_list ** send_opt_list)
{
    char wanoe_enable[BUFLEN_16] = {0};

    if ((req_opt_list == NULL) || (send_opt_list == NULL))
    {
        DBG_PRINT ("%s %d: Invalid args..\n", __FUNCTION__, __LINE__);
        return FAILURE;
    }

    //syscfg for eth_wan_enabled
    if (syscfg_get(NULL, "eth_wan_enabled", wanoe_enable, sizeof(wanoe_enable)) == 0)
    {
        if (strcmp(wanoe_enable, "true") == 0)
        {
            add_dhcpv4_opt_to_list(req_opt_list, DHCPV4_OPT_122, NULL);
            add_dhcpv4_opt_to_list(send_opt_list, DHCPV4_OPT_125, NULL);
        }
    }
    else
    {
        DBG_PRINT("Failed to get eth_wan_enabled \n");
    }
/* FIXME : Platform Hall Call in Future Reference
#ifndef __PLATFORM_HAL_H__
    if (platform_hal_GetDhcpv4_Options(req_opt_list, send_opt_list) == FAILURE)
    {
        DBG_PRINT("%s %d: failed to get option list from platform hal\n", __FUNCTION__, __LINE__);
        return FAILURE;
    }
#endif
*/
    return SUCCESS;

}
#endif

/*
 * start_dhcpv4_client ()
 * @description: This API will build dhcp request/send options and start dhcp client program.
 * @params     : input parameter to pass interface specific arguments
 * @return     : returns the pid of the dhcp client program else return error code on failure
 *
 */
pid_t start_dhcpv4_client (dhcp_params * params,dhcp_opt_list * req_opt_list,dhcp_opt_list * send_opt_list)
{
    if (params == NULL)
    {
        DBG_PRINT("%s %d: Invalid args..\n", __FUNCTION__, __LINE__);
        return 0;
    }

    if (req_opt_list == NULL)
    {
        DBG_PRINT("%s %d: Invalid args for req_opt_list..\n", __FUNCTION__, __LINE__);
    }
    if (send_opt_list == NULL)
    {
        DBG_PRINT("%s %d: Invalid args for send_opt_list..\n", __FUNCTION__, __LINE__);
    }

    pid_t pid = FAILURE;

    char mapt_mode[16] = {0};

    ifl_get_event("map_transport_mode", mapt_mode, sizeof(mapt_mode));
    if (strcmp(mapt_mode, "MAPT") == 0)
    {
        DBG_PRINT("%s: Do not start dhcpv4 client when mapt is already configured\n", __FUNCTION__);
        return pid;
    }


    // building args and starting dhcpv4 client
    DBG_PRINT("%s %d: Starting DHCP Clients\n", __FUNCTION__, __LINE__);
#ifdef DHCPV4_CLIENT_UDHCPC
    if (params->ifType == WAN_LOCAL_IFACE)
    {
    pid =  start_udhcpc (params, req_opt_list, send_opt_list);
    }
    else
    {
        // for REMOTE_IFACE,
        //  DHCP request options are needed
        //  DHCP send options are not necessary
        pid =  start_udhcpc (params, req_opt_list, NULL);
    }
#elif DHCPV4_CLIEN_TI_UDHCPC
    pid =  start_ti_udhcpc (params);
#endif

    //exit part
    DBG_PRINT("%s %d: freeing all allocated resources\n", __FUNCTION__, __LINE__);
    return pid;

}


/*
 * stop_dhcpv4_client ()
 * @description: This API will stop DHCP client running for interface specified in parameter
 * @params     : input parameter to pass interface specific arguments
 * @return     : SUCCESS if client is filled, else returns failure
 *
 */
int stop_dhcpv4_client (dhcp_params * params)
{
    if (params == NULL)
    {
        DBG_PRINT("%s %d: Invalid args..\n", __FUNCTION__, __LINE__);
        return FAILURE;
    }

#ifdef DHCPV4_CLIENT_UDHCPC
    return stop_udhcpc (params);
#endif
    return SUCCESS;

}

