##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
#
# Copyright 2020 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################

AM_CFLAGS = -D_ANSC_LINUX
AM_CFLAGS += -D_ANSC_USER
AM_LDFLAGS = -lccsp_common

AM_LDFLAGS += -lhal_plaform -lsysevent -lutapi -lutctx -lulog
AM_CPPFLAGS = -Wall -Werror
ACLOCAL_AMFLAGS = -I m4
hardware_platform = i686-linux-gnu
bin_PROGRAMS = CcspDHCPMgr

CcspDHCPMgr_CPPFLAGS =  -I$(top_srcdir)/source/TR-181/include -I$(top_srcdir)/source/TR-181/middle_layer_src $(CPPFLAGS) \
                        -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rbus \
                        -I$(top_srcdir)/source/DHCPClientUtils \
                        -I$(top_srcdir)/source/DHCPServerUtils/include	

CcspDHCPMgr_DEPENDENCIES= \
        $(EXTRA_DEPENDENCIES) \
        ${top_builddir}/source/TR-181/middle_layer_src/libCcspDhcpMgr_middle_layer_src.la \
        ${top_builddir}/source/TR-181/board_sbapi/libCcspDhcpMgr_board_sbapi.la \
        ${top_builddir}/source/DHCPClientUtils/libdhcp_client_utils.la \
        ${top_builddir}/source/DHCPServerUtils/libdhcp_server_utils.la

CcspDHCPMgr_CFLAGS = $(SYSTEMD_CFLAGS) $(NONROOT_CFLAGS)
CcspDHCPMgr_SOURCES = ssp_messagebus_interface.c ssp_main.c ssp_action.c dm_pack_datamodel.c
CcspDHCPMgr_LDFLAGS = -rdynamic $(SYSTEMD_LDFLAGS) -pthread
CcspDHCPMgr_LDADD =  $(CcspDHCPMgr_DEPENDENCIES) -lccsp_common -lsyscfg -ldl -lrbus
