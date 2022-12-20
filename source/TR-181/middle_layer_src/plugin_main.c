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

/***********************************************************************

    module: plugin_main.c

        Implement COSA Data Model Library Init and Unload apis.

    ---------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    ---------------------------------------------------------------

    revision:

        09/28/2011    initial revision.

**********************************************************************/

#include "ansc_platform.h"
#include "ansc_load_library.h"
#include "cosa_plugin_api.h"
#include "plugin_main.h"
#include "plugin_main_apis.h"
#include "cosa_dhcpv4_dml.h"
#include "cosa_dhcpv6_dml.h"
#include "cosa_dhcpv4_internal.h"
#include "cosa_dhcpv6_internal.h"
#include "safec_lib_common.h"
#include "cosa_common_util.h"

#include "cosa_dns_dml.h"
#include "cosa_dns_internal.h"
#include "cosa_dml_api_dns.h"

#include "cosa_webconfig_api.h"
#include "cosa_x_cisco_com_devicecontrol_internal.h"

#define THIS_PLUGIN_VERSION                         1

ANSC_HANDLE g_Dhcpv4Object ;
ANSC_HANDLE g_Dhcpv6Object ;

ANSC_HANDLE g_DnsObject;
ANSC_HANDLE g_DevCtlObject;

COSAGetParamValueByPathNameProc    g_GetParamValueByPathNameProc;
COSASetParamValueByPathNameProc    g_SetParamValueByPathNameProc;
//COSAGetParamValueStringProc        g_GetParamValueString;
COSAGetParamValueUlongProc         g_GetParamValueUlong;
COSAGetParamValueIntProc           g_GetParamValueInt;
COSAGetParamValueBoolProc          g_GetParamValueBool;
COSASetParamValueStringProc        g_SetParamValueString;
COSASetParamValueUlongProc         g_SetParamValueUlong;
COSASetParamValueIntProc           g_SetParamValueInt;
COSASetParamValueBoolProc          g_SetParamValueBool;
COSAGetInstanceNumbersProc         g_GetInstanceNumbers;

COSAValidateHierarchyInterfaceProc g_ValidateInterface;
COSAGetHandleProc                  g_GetRegistryRootFolder;
COSAGetInstanceNumberByIndexProc   g_GetInstanceNumberByIndex;
COSAGetInterfaceByNameProc         g_GetInterfaceByName;
COSAGetHandleProc                  g_GetMessageBusHandle;
COSAGetSubsystemPrefixProc         g_GetSubsystemPrefix;
PCCSP_CCD_INTERFACE                g_pPnmCcdIf;
ANSC_HANDLE                        g_MessageBusHandle;
char*                              g_SubsystemPrefix;
COSARegisterCallBackAfterInitDmlProc  g_RegisterCallBackAfterInitDml;
COSARepopulateTableProc            g_COSARepopulateTable;

//void *                       g_pDslhDmlAgent;
extern ANSC_HANDLE     g_MessageBusHandle_Irep;
extern char            g_SubSysPrefix_Irep[32];
PCOSA_BACKEND_MANAGER_OBJECT g_pCosaBEManager;

int ANSC_EXPORT_API
COSA_Init
(
    ULONG                       uMaxVersionSupported,
    void*                       hCosaPlugInfo         /* PCOSA_PLUGIN_INFO passed in by the caller */
    )
{
    PCOSA_PLUGIN_INFO               pPlugInfo  = (PCOSA_PLUGIN_INFO)hCosaPlugInfo;

    COSAGetParamValueByPathNameProc pGetParamValueByPathNameProc = (COSAGetParamValueByPathNameProc)NULL;
    COSASetParamValueByPathNameProc pSetParamValueByPathNameProc = (COSASetParamValueByPathNameProc)NULL;
    //COSAGetParamValueStringProc     pGetStringProc              = (COSAGetParamValueStringProc       )NULL;
    COSAGetParamValueUlongProc      pGetParamValueUlongProc     = (COSAGetParamValueUlongProc        )NULL;
    COSAGetParamValueIntProc        pGetParamValueIntProc       = (COSAGetParamValueIntProc          )NULL;
    COSAGetParamValueBoolProc       pGetParamValueBoolProc      = (COSAGetParamValueBoolProc         )NULL;
    COSASetParamValueStringProc     pSetStringProc              = (COSASetParamValueStringProc       )NULL;
    COSASetParamValueUlongProc      pSetParamValueUlongProc     = (COSASetParamValueUlongProc        )NULL;
    COSASetParamValueIntProc        pSetParamValueIntProc       = (COSASetParamValueIntProc          )NULL;
    COSASetParamValueBoolProc       pSetParamValueBoolProc      = (COSASetParamValueBoolProc         )NULL;
    COSAGetInstanceNumbersProc      pGetInstanceNumbersProc     = (COSAGetInstanceNumbersProc        )NULL;

    COSAValidateHierarchyInterfaceProc
        pValInterfaceProc           = (COSAValidateHierarchyInterfaceProc)NULL;
    COSAGetHandleProc               pGetRegistryRootFolder      = (COSAGetHandleProc                 )NULL;
    COSAGetInstanceNumberByIndexProc
        pGetInsNumberByIndexProc    = (COSAGetInstanceNumberByIndexProc  )NULL;
    COSAGetInterfaceByNameProc      pGetInterfaceByNameProc     = (COSAGetInterfaceByNameProc        )NULL;
    errno_t        rc = -1;

    if ( uMaxVersionSupported < THIS_PLUGIN_VERSION )
    {
        /* this version is not supported */
        return -1;
    }

    pPlugInfo->uPluginVersion       = THIS_PLUGIN_VERSION;
    /* register the back-end apis for the data model */
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "DHCPv4_GetParamBoolValue",  DHCPv4_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "DHCPv4_GetParamIntValue",  DHCPv4_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "DHCPv4_GetParamUlongValue",  DHCPv4_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "DHCPv4_GetParamStringValue", DHCPv4_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_GetEntryCount", Client_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_GetEntry", Client_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_AddEntry", Client_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_DelEntry", Client_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_GetParamBoolValue", Client_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_GetParamIntValue", Client_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_GetParamUlongValue", Client_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_GetParamStringValue", Client_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_SetParamBoolValue", Client_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_SetParamIntValue", Client_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_SetParamUlongValue", Client_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_SetParamStringValue", Client_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_Validate", Client_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_Commit", Client_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client_Rollback", Client_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_GetEntryCount", SentOption_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_GetEntry", SentOption_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_AddEntry", SentOption_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_DelEntry", SentOption_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_GetParamBoolValue", SentOption_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_GetParamIntValue", SentOption_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_GetParamUlongValue", SentOption_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_GetParamStringValue", SentOption_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_SetParamBoolValue", SentOption_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_SetParamIntValue", SentOption_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_SetParamUlongValue", SentOption_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_SetParamStringValue", SentOption_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_Validate", SentOption_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_Commit", SentOption_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption_Rollback", SentOption_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_GetEntryCount", ReqOption_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_GetEntry", ReqOption_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_AddEntry", ReqOption_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_DelEntry", ReqOption_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_GetParamBoolValue", ReqOption_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_GetParamIntValue", ReqOption_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_GetParamUlongValue", ReqOption_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_GetParamStringValue", ReqOption_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_SetParamBoolValue", ReqOption_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_SetParamIntValue", ReqOption_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_SetParamUlongValue", ReqOption_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_SetParamStringValue", ReqOption_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_Validate", ReqOption_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_Commit", ReqOption_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReqOption_Rollback", ReqOption_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_GetParamBoolValue", Server_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_GetParamIntValue", Server_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_GetParamUlongValue", Server_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_GetParamStringValue", Server_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_SetParamBoolValue", Server_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_SetParamIntValue", Server_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_SetParamUlongValue", Server_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_SetParamStringValue", Server_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_Validate", Server_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_Commit", Server_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server_Rollback", Server_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_GetEntryCount", Pool_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_GetEntry", Pool_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_AddEntry", Pool_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_DelEntry", Pool_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_GetParamBoolValue", Pool_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_GetParamIntValue", Pool_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_GetParamUlongValue", Pool_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_GetParamStringValue", Pool_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_SetParamBoolValue", Pool_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_SetParamIntValue", Pool_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_SetParamUlongValue", Pool_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_SetParamStringValue", Pool_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_Validate", Pool_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_Commit", Pool_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool_Rollback", Pool_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_GetEntryCount", StaticAddress_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_GetEntry", StaticAddress_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_IsUpdated", StaticAddress_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_Synchronize", StaticAddress_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_AddEntry", StaticAddress_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_DelEntry", StaticAddress_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_GetParamBoolValue", StaticAddress_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_GetParamIntValue", StaticAddress_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_GetParamUlongValue", StaticAddress_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_GetParamStringValue", StaticAddress_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_SetParamBoolValue", StaticAddress_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_SetParamIntValue", StaticAddress_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_SetParamUlongValue", StaticAddress_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_SetParamStringValue", StaticAddress_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_Validate", StaticAddress_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_Commit", StaticAddress_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "StaticAddress_Rollback", StaticAddress_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_GetEntryCount", Option1_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_GetEntry", Option1_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_AddEntry", Option1_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_DelEntry", Option1_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_GetParamBoolValue", Option1_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_GetParamIntValue", Option1_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_GetParamUlongValue", Option1_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_GetParamStringValue", Option1_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_SetParamBoolValue", Option1_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_SetParamIntValue", Option1_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_SetParamUlongValue", Option1_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_SetParamStringValue", Option1_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_Validate", Option1_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_Commit", Option1_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option1_Rollback", Option1_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_GetEntryCount", Client2_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_GetEntry", Client2_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_IsUpdated", Client2_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_Synchronize", Client2_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_GetParamBoolValue", Client2_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_GetParamIntValue", Client2_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_GetParamUlongValue", Client2_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_GetParamStringValue", Client2_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_SetParamBoolValue", Client2_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_SetParamIntValue", Client2_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_SetParamUlongValue", Client2_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_SetParamStringValue", Client2_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_Validate", Client2_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_Commit", Client2_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client2_Rollback", Client2_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv4Address2_GetEntryCount", IPv4Address2_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv4Address2_GetEntry", IPv4Address2_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv4Address2_GetParamBoolValue", IPv4Address2_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv4Address2_GetParamIntValue", IPv4Address2_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv4Address2_GetParamUlongValue", IPv4Address2_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv4Address2_GetParamStringValue", IPv4Address2_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option2_GetEntryCount", Option2_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option2_GetEntry", Option2_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option2_GetParamBoolValue", Option2_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option2_GetParamIntValue", Option2_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option2_GetParamUlongValue", Option2_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option2_GetParamStringValue", Option2_GetParamStringValue);




    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "DHCPv6_GetParamBoolValue", DHCPv6_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "DHCPv6_GetParamIntValue", DHCPv6_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "DHCPv6_GetParamUlongValue", DHCPv6_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "DHCPv6_GetParamStringValue", DHCPv6_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_GetEntryCount", Client3_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_GetEntry", Client3_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_AddEntry", Client3_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_DelEntry", Client3_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_GetParamBoolValue", Client3_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_GetParamIntValue", Client3_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_GetParamUlongValue", Client3_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_GetParamStringValue", Client3_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_SetParamBoolValue", Client3_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_SetParamIntValue", Client3_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_SetParamUlongValue", Client3_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_SetParamStringValue", Client3_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_Validate", Client3_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_Commit", Client3_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client3_Rollback", Client3_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server2_GetEntryCount", Server2_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server2_GetEntry", Server2_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server2_IsUpdated", Server2_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server2_Synchronize", Server2_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server2_GetParamBoolValue", Server2_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server2_GetParamIntValue", Server2_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server2_GetParamUlongValue", Server2_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server2_GetParamStringValue", Server2_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_GetEntryCount", SentOption1_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_GetEntry", SentOption1_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_AddEntry", SentOption1_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_DelEntry", SentOption1_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_GetParamBoolValue", SentOption1_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_GetParamIntValue", SentOption1_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_GetParamUlongValue", SentOption1_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_GetParamStringValue", SentOption1_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_SetParamBoolValue", SentOption1_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_SetParamIntValue", SentOption1_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_SetParamUlongValue", SentOption1_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_SetParamStringValue", SentOption1_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_Validate", SentOption1_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_Commit", SentOption1_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "SentOption1_Rollback", SentOption1_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReceivedOption_GetEntryCount", ReceivedOption_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReceivedOption_GetEntry",ReceivedOption_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReceivedOption_IsUpdated", ReceivedOption_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReceivedOption_Synchronize", ReceivedOption_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReceivedOption_GetParamBoolValue", ReceivedOption_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReceivedOption_GetParamIntValue", ReceivedOption_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReceivedOption_GetParamUlongValue", ReceivedOption_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "ReceivedOption_GetParamStringValue", ReceivedOption_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "dhcp6c_mapt_mape_GetParamBoolValue", dhcp6c_mapt_mape_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "dhcp6c_mapt_mape_GetParamUlongValue", dhcp6c_mapt_mape_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "dhcp6c_mapt_mape_GetParamStringValue", dhcp6c_mapt_mape_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_GetParamBoolValue", Server3_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_GetParamIntValue", Server3_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_GetParamUlongValue", Server3_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_GetParamStringValue", Server3_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_SetParamBoolValue",Server3_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_SetParamIntValue", Server3_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_SetParamUlongValue", Server3_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_SetParamStringValue", Server3_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_Validate", Server3_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_Commit", Server3_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server3_Rollback", Server3_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_GetEntryCount", Pool1_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_GetEntry", Pool1_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_AddEntry", Pool1_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_DelEntry", Pool1_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_GetParamBoolValue", Pool1_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_GetParamUlongValue", Pool1_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_GetParamStringValue", Pool1_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_SetParamBoolValue", Pool1_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_SetParamUlongValue", Pool1_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_SetParamStringValue", Pool1_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_Validate", Pool1_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_Commit", Pool1_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Pool1_Rollback", Pool1_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_GetEntryCount", Client4_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_GetEntry", Client4_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_IsUpdated", Client4_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_Synchronize", Client4_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_GetParamBoolValue", Client4_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_GetParamIntValue", Client4_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_GetParamUlongValue", Client4_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_GetParamStringValue", Client4_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_SetParamBoolValue", Client4_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_SetParamIntValue", Client4_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_SetParamUlongValue", Client4_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_SetParamStringValue", Client4_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_Validate", Client4_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_Commit", Client4_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client4_Rollback", Client4_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Address2_GetEntryCount", IPv6Address2_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Address2_GetEntry", IPv6Address2_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Address2_GetParamBoolValue", IPv6Address2_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Address2_GetParamIntValue", IPv6Address2_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Address2_GetParamUlongValue", IPv6Address2_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Address2_GetParamStringValue",IPv6Address2_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Prefix1_GetEntryCount", IPv6Prefix1_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Prefix1_GetEntry", IPv6Prefix1_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Prefix1_GetParamBoolValue", IPv6Prefix1_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Prefix1_GetParamIntValue", IPv6Prefix1_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Prefix1_GetParamUlongValue", IPv6Prefix1_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "IPv6Prefix1_GetParamStringValue", IPv6Prefix1_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option3_GetEntryCount", Option3_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option3_GetEntry", Option3_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option3_GetParamBoolValue", Option3_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option3_GetParamIntValue", Option3_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option3_GetParamUlongValue", Option3_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option3_GetParamStringValue", Option3_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_GetEntryCount", Option4_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_GetEntry", Option4_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_AddEntry", Option4_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_DelEntry", Option4_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_GetParamBoolValue", Option4_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_GetParamIntValue", Option4_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_GetParamUlongValue", Option4_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_GetParamStringValue", Option4_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_SetParamBoolValue", Option4_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_SetParamIntValue", Option4_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_SetParamUlongValue", Option4_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_SetParamStringValue", Option4_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_Validate", Option4_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_Commit", Option4_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Option4_Rollback", Option4_Rollback);



    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_GetParamBoolValue", Client1_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_GetParamIntValue", Client1_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_GetParamUlongValue", Client1_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_GetParamStringValue", Client1_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_SetParamBoolValue", Client1_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_SetParamIntValue", Client1_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_SetParamUlongValue", Client1_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_SetParamStringValue", Client1_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_Validate", Client1_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_Commit", Client1_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Client1_Rollback", Client1_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_GetEntryCount", Server1_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_GetEntry", Server1_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_AddEntry", Server1_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_DelEntry", Server1_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_IsUpdated", Server1_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_Synchronize", Server1_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_GetParamBoolValue", Server1_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_GetParamIntValue", Server1_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_GetParamUlongValue", Server1_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_GetParamStringValue", Server1_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_SetParamBoolValue", Server1_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_SetParamIntValue", Server1_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_SetParamUlongValue", Server1_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_SetParamStringValue", Server1_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_Validate", Server1_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_Commit", Server1_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Server1_Rollback", Server1_Rollback);

    g_pDslhDmlAgent                 = pPlugInfo->hDmlAgent;
    pGetParamValueByPathNameProc = (COSAGetParamValueByPathNameProc)pPlugInfo->AcquireFunction("COSAGetParamValueByPathName");
    if( pGetParamValueByPathNameProc != NULL)
    {
        g_GetParamValueByPathNameProc = pGetParamValueByPathNameProc;
    }
    else
    {
        goto EXIT;
    }
    pSetParamValueByPathNameProc = (COSASetParamValueByPathNameProc)pPlugInfo->AcquireFunction("COSASetParamValueByPathName");

    if( pSetParamValueByPathNameProc != NULL)
    {
        g_SetParamValueByPathNameProc = pSetParamValueByPathNameProc;
    }
    else
    {
        goto EXIT;
    }
    /*
      pGetStringProc = (COSAGetParamValueStringProc)pPlugInfo->AcquireFunction("COSAGetParamValueString");

      if( pGetStringProc != NULL)
      {
      g_GetParamValueString = pGetStringProc;
      }
      else
      {
      goto EXIT;
      }
    */
    pGetParamValueUlongProc = (COSAGetParamValueUlongProc)pPlugInfo->AcquireFunction("COSAGetParamValueUlong");

    if( pGetParamValueUlongProc != NULL)
    {
        g_GetParamValueUlong = pGetParamValueUlongProc;
    }
    else
    {
        goto EXIT;
    }

    pGetParamValueIntProc = (COSAGetParamValueIntProc)pPlugInfo->AcquireFunction("COSAGetParamValueInt");

    if( pGetParamValueIntProc != NULL)
    {
        g_GetParamValueInt = pGetParamValueIntProc;
    }
    else
    {
        goto EXIT;
    }
    pGetParamValueBoolProc = (COSAGetParamValueBoolProc)pPlugInfo->AcquireFunction("COSAGetParamValueBool");

    if( pGetParamValueBoolProc != NULL)
    {
        g_GetParamValueBool = pGetParamValueBoolProc;
    }
    else
    {
        goto EXIT;
    }
    pSetStringProc = (COSASetParamValueStringProc)pPlugInfo->AcquireFunction("COSASetParamValueString");

    if( pSetStringProc != NULL)
    {
        g_SetParamValueString = pSetStringProc;
    }
    else
    {
        goto EXIT;
    }
    pSetParamValueUlongProc = (COSASetParamValueUlongProc)pPlugInfo->AcquireFunction("COSASetParamValueUlong");

    if( pSetParamValueUlongProc != NULL)
    {
        g_SetParamValueUlong = pSetParamValueUlongProc;
    }
    else
    {
        goto EXIT;
    }

    pSetParamValueIntProc = (COSASetParamValueIntProc)pPlugInfo->AcquireFunction("COSASetParamValueInt");

    if( pSetParamValueIntProc != NULL)
    {
        g_SetParamValueInt = pSetParamValueIntProc;
    }
    else
    {
        goto EXIT;
    }
    pSetParamValueBoolProc = (COSASetParamValueBoolProc)pPlugInfo->AcquireFunction("COSASetParamValueBool");

    if( pSetParamValueBoolProc != NULL)
    {
        g_SetParamValueBool = pSetParamValueBoolProc;
    }
    else
    {
        goto EXIT;
    }
    pGetInstanceNumbersProc = (COSAGetInstanceNumbersProc)pPlugInfo->AcquireFunction("COSAGetInstanceNumbers");

    if( pGetInstanceNumbersProc != NULL)
    {
        g_GetInstanceNumbers = pGetInstanceNumbersProc;
    }
    else
    {
        goto EXIT;
    }
    pValInterfaceProc = (COSAValidateHierarchyInterfaceProc)pPlugInfo->AcquireFunction("COSAValidateHierarchyInterface");

    if ( pValInterfaceProc )
    {
        g_ValidateInterface = pValInterfaceProc;
    }
    else
    {
        goto EXIT;
    }
    pGetRegistryRootFolder = (COSAGetHandleProc)pPlugInfo->AcquireFunction("COSAGetRegistryRootFolder");

    if ( pGetRegistryRootFolder != NULL )
    {
        g_GetRegistryRootFolder = pGetRegistryRootFolder;
    }
    else
    {
        CcspTraceInfo(("!!! haha, catcha !!!\n"));
        goto EXIT;
    }
    pGetInsNumberByIndexProc = (COSAGetInstanceNumberByIndexProc)pPlugInfo->AcquireFunction("COSAGetInstanceNumberByIndex");

    if ( pGetInsNumberByIndexProc != NULL )
    {
        g_GetInstanceNumberByIndex = pGetInsNumberByIndexProc;
    }
    else
    {
        goto EXIT;
    }
    pGetInterfaceByNameProc = (COSAGetInterfaceByNameProc)pPlugInfo->AcquireFunction("COSAGetInterfaceByName");

    if ( pGetInterfaceByNameProc != NULL )
    {
        g_GetInterfaceByName = pGetInterfaceByNameProc;
    }
    else
    {
        goto EXIT;
    }
    g_pPnmCcdIf = g_GetInterfaceByName(g_pDslhDmlAgent, CCSP_CCD_INTERFACE_NAME);

    if ( !g_pPnmCcdIf )
    {
        CcspTraceError(("g_pPnmCcdIf is NULL !\n"));

        goto EXIT;
    }
    g_RegisterCallBackAfterInitDml = (COSARegisterCallBackAfterInitDmlProc)pPlugInfo->AcquireFunction("COSARegisterCallBackAfterInitDml");

    if ( !g_RegisterCallBackAfterInitDml )
    {
        goto EXIT;
    }
    g_COSARepopulateTable = (COSARepopulateTableProc)pPlugInfo->AcquireFunction("COSARepopulateTable");

    if ( !g_COSARepopulateTable )
    {
        goto EXIT;
    }
    /* Get Message Bus Handle */
    g_GetMessageBusHandle = (COSAGetHandleProc)pPlugInfo->AcquireFunction("COSAGetMessageBusHandle");
    if ( g_GetMessageBusHandle == NULL )
    {
        goto EXIT;
    }
    g_MessageBusHandle = (ANSC_HANDLE)g_GetMessageBusHandle(g_pDslhDmlAgent);
    if ( g_MessageBusHandle == NULL )
    {
        goto EXIT;
    }
    g_MessageBusHandle_Irep = g_MessageBusHandle;

    /* Get Subsystem prefix */
    g_GetSubsystemPrefix = (COSAGetSubsystemPrefixProc)pPlugInfo->AcquireFunction("COSAGetSubsystemPrefix");
    if ( g_GetSubsystemPrefix != NULL )
    {
        char*   tmpSubsystemPrefix;

        if (( tmpSubsystemPrefix = g_GetSubsystemPrefix(g_pDslhDmlAgent) ))
        {
            rc =  strcpy_s(g_SubSysPrefix_Irep,sizeof(g_SubSysPrefix_Irep) ,tmpSubsystemPrefix);
            if(rc != EOK)
            {
                ERR_CHK(rc);
                return -1;
            }
        }

        /* retrieve the subsystem prefix */
        g_SubsystemPrefix = g_GetSubsystemPrefix(g_pDslhDmlAgent);
    }

    g_Dhcpv4Object       = (ANSC_HANDLE)CosaDhcpv4Create();
    g_Dhcpv6Object       = (ANSC_HANDLE)CosaDhcpv6Create();

#if 0
    g_DnsObject          = (ANSC_HANDLE)CosaDNSCreate();
#endif
    g_DevCtlObject       = (ANSC_HANDLE)CosaDeviceControlCreate();

    EvtDispterHandleEventAsync();
    webConfigFrameworkInit();

    return  0;
EXIT:

    return -1;

}

BOOL ANSC_EXPORT_API
COSA_IsObjectSupported
(
    char*                        pObjName
    )
{
    UNREFERENCED_PARAMETER(pObjName);
    return TRUE;
}

void ANSC_EXPORT_API
COSA_Unload
(
    void
    )
{
    /* unload the memory here */
}
