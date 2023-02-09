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

/*********************************************************************************

    description:

        This is the template file of ssp_main.c for XxxxSsp.
        Please replace "XXXX" with your own ssp name with the same up/lower cases.

  ------------------------------------------------------------------------------

    revision:

        09/08/2011    initial revision.

**********************************************************************************/


#ifdef __GNUC__
#ifndef _BUILD_ANDROID
#include <execinfo.h>
#endif
#endif

#include "ssp_global.h"
#ifdef INCLUDE_BREAKPAD
#include "breakpad_wrapper.h"
#endif
#include "stdlib.h"
#include "ccsp_dm_api.h"

#define DEBUG_INI_NAME  "/etc/debug.ini"
#include "syscfg/syscfg.h"
#include "cap.h"
#include "safec_lib_common.h"
#include "ifl.h"

#ifdef DHCPV4_SERVER_SUPPORT
#include "service_dhcp_server.h"
#include "dhcpv4_server_interface.h"
#endif

#ifdef DHCPV4_CLIENT_SUPPORT
#include "service_dhcpv4_client.h"
#endif

#ifdef DHCPV6_CLIENT_SUPPORT
#include "service_dhcpv6_client.h"
#endif

#ifdef DHCPV6_SERVER_SUPPORT
extern void dhcpv6_server_init();
#endif

extern void remove_file(char *);

static cap_user appcaps;

int GetLogInfo(ANSC_HANDLE bus_handle, char *Subsytem, char *pParameterName);
extern char*                                pComponentName;
char                                        g_Subsystem[32]         = {0};
extern ANSC_HANDLE bus_handle;

int  cmd_dispatch(int  command)
{
  
  errno_t rc = -1;
    switch ( command )
    {
        case    'e' :

            CcspTraceInfo(("Connect to bus daemon...\n"));

            {
                char                            CName[256];

                if ( g_Subsystem[0] != 0 )
                {
                   rc = sprintf_s(CName, sizeof(CName), "%s%s", g_Subsystem, CCSP_COMPONENT_ID_DHCPMGR);
                  if (rc < EOK)
                  {
                  ERR_CHK(rc);
                  return -1;
                  }
                  
                }
                else
                {
                    rc = sprintf_s(CName, sizeof(CName), "%s", CCSP_COMPONENT_ID_DHCPMGR);
                   if (rc < EOK)
                  {
                  ERR_CHK(rc);
                  return -1;
                  }
                }

                ssp_Mbi_MessageBusEngage
                    ( 
                        CName,
                        CCSP_MSG_BUS_CFG,
                        CCSP_COMPONENT_PATH_DHCPMGR
                    );
            }

            ssp_create();
            ssp_engage();

            break;

        case    'm':

                AnscPrintComponentMemoryTable(pComponentName);

                break;

        case    't':

                AnscTraceMemoryTable();

                break;

        case    'c':
                
                ssp_cancel();

                break;

        default:
            break;
    }

    return 0;
}

static void _print_stack_backtrace(void)
{
#ifdef __GNUC__
#ifndef _BUILD_ANDROID
        void* tracePtrs[100];
        char** funcNames = NULL;
        int i, count = 0;

        count = backtrace( tracePtrs, 100 );
        backtrace_symbols_fd( tracePtrs, count, 2 );

        funcNames = backtrace_symbols( tracePtrs, count );

        if ( funcNames ) {
            // Print the stack trace
            for( i = 0; i < count; i++ )
            CcspTraceWarning(("%s\n", funcNames[i] ));

            // Free the string pointers
            free( funcNames );
        }
#endif
#endif
}

static void daemonize(void) {
        switch (fork()) {
        case 0:
             break;
        case -1:
              // Error
               CcspTraceInfo(("Error daemonizing (fork)! %d - %s\n", errno, strerror(
                            errno)));
                exit(0);
                break;
        default:
                _exit(0);
        }

        if (setsid() < 0) {
                CcspTraceInfo(("Error demonizing (setsid)! %d - %s\n", errno, strerror(errno)));
                exit(0);
        }

//      chdir("/");


#ifndef  _DEBUG
        int fd;
        fd = open("/dev/null", O_RDONLY);
        if (fd != 0) {
                dup2(fd, 0);
                close(fd);
        }
        fd = open("/dev/null", O_WRONLY);
        if (fd != 1) {
                dup2(fd, 1);
                close(fd);
        }
        fd = open("/dev/null", O_WRONLY);
        if (fd != 2) {
                dup2(fd, 2);
                close(fd);
        }
#endif
}

void sig_handler(int sig)
{
    if ( sig == SIGINT ) {
        signal(SIGINT, sig_handler); /* reset it to this function */
        CcspTraceInfo(("SIGINT received!\n"));
        exit(0);
    }
    else if ( sig == SIGUSR1 ) {
        signal(SIGUSR1, sig_handler); /* reset it to this function */
        CcspTraceInfo(("SIGUSR1 received!\n"));
    }
    else if ( sig == SIGUSR2 ) {
        CcspTraceInfo(("SIGUSR2 received!\n"));
    }
    else if ( sig == SIGCHLD ) {
        signal(SIGCHLD, sig_handler); /* reset it to this function */
        CcspTraceInfo(("SIGCHLD received!\n"));
    }
    else if ( sig == SIGPIPE ) {
        signal(SIGPIPE, sig_handler); /* reset it to this function */
        CcspTraceInfo(("SIGPIPE received!\n"));
    }
    else if ( sig == SIGALRM ) 
    {

        signal(SIGALRM, sig_handler); /* reset it to this function */
        CcspTraceInfo(("SIGALRM received!\n"));
    }
    else {
        /* get stack trace first */
        _print_stack_backtrace();
        CcspTraceInfo(("Signal %d received, exiting!\n", sig));
        exit(0);
    }

}


#ifndef INCLUDE_BREAKPAD
static int is_core_dump_opened(void)
{
    FILE *fp;
    char path[256];
    char line[1024];
    char *start, *tok, *sp;
#define TITLE   "Max core file size"

    snprintf(path, sizeof(path), "/proc/%d/limits", getpid());
    if ((fp = fopen(path, "rb")) == NULL)
        return 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if ((start = strstr(line, TITLE)) == NULL)
            continue;

        start += strlen(TITLE);
        if ((tok = strtok_r(start, " \t\r\n", &sp)) == NULL)
            break;

        fclose(fp);
        return (tok[0] == '0' && tok[1] == '\0') ? 0 : 1;
    }

    fclose(fp);
    return 0;
}
#endif
#if 0
static bool drop_root()
{
  appcaps.caps = NULL;
  appcaps.user_name = NULL;
  bool retval = false;
  bool ret = false;
  ret = isBlocklisted();
  if(ret)
  {
    CcspTraceInfo(("NonRoot feature is disabled\n"));
  }
  else
  {
    CcspTraceInfo(("NonRoot feature is enabled, dropping root privileges for CcspDHCPMgr process\n"));
    if(init_capability() != NULL) {
      if(drop_root_caps(&appcaps) != -1) {
        if(update_process_caps(&appcaps) != -1) {
          read_capability(&appcaps);
          retval = true;
        }
      }
    }
  }
  return retval;
}
#endif
int main(int argc, char* argv[])
{
    BOOL                            bRunAsDaemon       = TRUE;
    int                             cmdChar            = 0;
    int                             idx = 0;
    appcaps.caps = NULL;
    appcaps.user_name = NULL;
    //char buf[8] = {'\0'};
    extern ANSC_HANDLE bus_handle;
    char *subSys            = NULL;  
    DmErr_t    err;
    errno_t        rc = -1;
    int ind = -1;

    CcspTraceInfo(("\nWithin the main function\n"));

#ifdef FEATURE_SUPPORT_RDKLOG
    RDK_LOGGER_INIT();
#endif

    if (access("/tmp/dhcpmgr_initialized", F_OK) == 0)
    {
        CcspTraceInfo(("/tmp/dhcpmgr_initialized already exists, removing it"));
        remove_file("/tmp/dhcpmgr_initialized");
    }

    for (idx = 1; idx < argc; idx++)
    {
         rc = strcmp_s("-subsys",strlen("-subsys"),argv[idx],&ind);
         ERR_CHK(rc);
         if((!ind) && (rc == EOK))
        {
           if ((idx+1) < argc)
           {
              rc = strcpy_s(g_Subsystem,sizeof(g_Subsystem), argv[idx+1]);
              if(rc != EOK)
             {
               ERR_CHK(rc);
               return ANSC_STATUS_FAILURE;
             }
           }
           else
           {
               CcspTraceError(("parameter after -subsys is missing"));
           }
             
        }
        else
        {
           rc = strcmp_s("-c", strlen("-c"),argv[idx],&ind );
           ERR_CHK(rc);
           if((!ind) && (rc == EOK))
          {
            bRunAsDaemon = FALSE;
          }
       }
    }

    pComponentName          = CCSP_COMPONENT_NAME_DHCPMGR;

#if 0
    if(!drop_root())
    {
        CcspTraceError(("drop_root function failed!\n"));
        gain_root_privilege();
    }
#endif
    if ( bRunAsDaemon ) 
        daemonize();

CcspTraceInfo(("\nAfter daemonize before signal\n"));

#ifdef INCLUDE_BREAKPAD
    breakpad_ExceptionHandler();
    signal(SIGUSR1, sig_handler);    
#else
    if (is_core_dump_opened())
    {
        signal(SIGUSR1, sig_handler);
        CcspTraceWarning(("Core dump is opened, do not catch signal\n"));
    }
    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);
    /*signal(SIGCHLD, sig_handler);*/
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);

    signal(SIGSEGV, sig_handler);
    signal(SIGBUS, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGFPE, sig_handler);
    signal(SIGILL, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGALRM, sig_handler);
#endif

    CcspTraceInfo(("DHCPMgr InterfaceLayer initialization started\n"));
    // inti dhcpmgr interfacelayer
    if (ifl_init("DHCP-Mgr") == IFL_SUCCESS)
    {
        if (IFL_SUCCESS != ifl_init_ctx("DHCP-Mgr-main", IFL_CTX_STATIC))
        {
            CcspTraceError(("Failed to init ifl ctx for DHCP-Mgr-main\n"));
        }
        CcspTraceInfo(("DHCPMgr InterfaceLayer initialized.\n"));
    }
    else
    {
        CcspTraceError(("Error in initialising DHCPMgr InterfaceLayer\n"));
    }

    CcspTraceInfo(("\nbefore cmd_dispatch command\n"));

    cmd_dispatch('e');

/*
    CcspTraceInfo(("DHCPMGR_DBG:-------Read Log Info\n"));
    char buffer[5] = {0};
    if( 0 == syscfg_get( NULL, "X_RDKCENTRAL-COM_LoggerEnable" , buffer, sizeof( buffer ) ) &&  ( buffer[0] != '\0' ) )
    {
        RDKLogEnable = (BOOL)atoi(buffer);
    }
    memset(buffer, 0, sizeof(buffer));
    if( 0 == syscfg_get( NULL, "X_RDKCENTRAL-COM_LogLevel" , buffer, sizeof( buffer ) ) &&  ( buffer[0] != '\0' ) )
    {
        RDKLogLevel = (ULONG )atoi(buffer);
    }
    memset(buffer, 0, sizeof(buffer));
    if( 0 == syscfg_get( NULL, "X_RDKCENTRAL-COM_DhcpMgr_LogLevel" , buffer, sizeof( buffer ) ) &&  ( buffer[0] != '\0' ) )
    {
        DHCPMGR_RDKLogLevel = (ULONG)atoi(buffer);
    }
    memset(buffer, 0, sizeof(buffer));
    if( 0 == syscfg_get( NULL, "X_RDKCENTRAL-COM_DhcpMgr_LoggerEnable" , buffer, sizeof( buffer ) ) &&  ( buffer[0] != '\0' ) )
    {
        DHCPMGR_RDKLogEnable = (BOOL)atoi(buffer);
    }
    CcspTraceInfo(("DHCPMGR_DBG:-------Log Info values RDKLogEnable:%d,RDKLogLevel:%u,DHCPMGR_RDKLogLevel:%u,DHCPMGR_RDKLogEnable:%d\n",RDKLogEnable,RDKLogLevel,DHCPMGR_RDKLogLevel, DHCPMGR_RDKLogEnable ));
*/ 
#ifdef _COSA_SIM_
    subSys = "";        /* PC simu use empty string as subsystem */
#else
    subSys = NULL;      /* use default sub-system */
#endif
CcspTraceWarning(("\nBefore Cdm_Init\n"));

    err = Cdm_Init(bus_handle, subSys, NULL, NULL, pComponentName);
CcspTraceWarning(("\nAfter Cdm_Init\n"));

    if (err != CCSP_SUCCESS)
    {
        fprintf(stderr, "Cdm_Init: %s\n", Cdm_StrError(err));
        exit(1);
    }

#ifdef DHCPV4_CLIENT_SUPPORT
    //Init dhcpv4 client
    CcspTraceInfo(("serv_dhcp_init (dhcpv4 client) Started\n"));
    serv_dhcp_init();
    CcspTraceInfo(("serv_dhcp_init (dhcpv4 client) Ended\n"));
#endif

#ifdef DHCPV6_CLIENT_SUPPORT
    //Init dhcpv6 cleint
    CcspTraceInfo(("init_dhcpv6_client (dhcpv6 cleint) Started\n"));
    init_dhcpv6_client ();
    CcspTraceInfo(("init_dhcpv6_client (dhcpv6 cleint) Ended\n"));
#endif

#ifdef DHCPV4_SERVER_SUPPORT
    //Init dhcpv4 server
    CcspTraceInfo(("dhcp_server_init (dhcpv4 server) Started\n"));
    dhcp_server_init();
    CcspTraceInfo(("dhcp_server_init (dhcpv4 server) Ended\n"));
#endif

#ifdef DHCPV6_SERVER_SUPPORT
    //Init dhcpv6 server
    CcspTraceInfo(("dhcp_server_init (dhcpv6 server) Started\n"));
    dhcpv6_server_init();
    CcspTraceInfo(("dhcp_server_init (dhcpv6 server) Ended\n"));
#endif
 
#ifdef DHCPV4_SERVER_SUPPORT
    //Init dhcp server services
    CcspTraceInfo(("init_dhcp_server_service Started\n"));
    init_dhcp_server_service();
    CcspTraceInfo(("init_dhcp_server_service Ended\n"));
#endif

    system("touch /tmp/dhcpmgr_initialized");

    ifl_deinit_ctx("DHCP-Mgr-main");

    if ( bRunAsDaemon )
    {
        while(1)
        {
            sleep(30);
        }
    }
    else
    {
        while ( cmdChar != 'q' )
        {
            cmdChar = getchar();

            cmd_dispatch(cmdChar);
        }
    }

        err = Cdm_Term();
        if (err != CCSP_SUCCESS)
        {
        fprintf(stderr, "Cdm_Term: %s\n", Cdm_StrError(err));
        exit(1);
        }
CcspTraceInfo(("\n Before ssp_cancel() \n"));
        ssp_cancel();
        //exit_rbus_service();
#if defined(_CBR_PRODUCT_REQ_) && !defined(_CBR2_PRODUCT_REQ_)
	serv_ipv6_term();
#endif
CcspTraceInfo(("\nExiting the main function\n"));
    return 0;
}
