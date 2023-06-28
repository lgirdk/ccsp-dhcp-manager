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

AM_CFLAGS =

ACLOCAL_AMFLAGS = -I m4

noinst_LTLIBRARIES = libCcspDhcpMgr_board_sbapi.la

libCcspDhcpMgr_board_sbapi_la_CPPFLAGS = -I$(top_srcdir)/source/TR-181/include \
                                         -I$(top_srcdir)/source/TR-181/middle_layer_src \
                                         -I$(top_srcdir)/source/DHCPMgrInterface/include \
                                         -I$(top_srcdir)/source/DHCPServerUtils/ \
                                         -I$(top_srcdir)/source/DHCPServerUtils/utils/include \
                                         -I$(top_srcdir)/source/DHCPClientUtils/DHCPv4Client/include \
                                         -I$(top_srcdir)/source/DHCPClientUtils/DHCPv6Client/include \
                                         -I$(top_srcdir)/source/DHCPServerUtils/DHCPv6Server \
                                         -I$(top_srcdir)/source/DHCPServerUtils/DHCPv6Server/include \
                                         -I$(top_srcdir)/source/DHCPMgrUtils/include

libCcspDhcpMgr_board_sbapi_la_SOURCES = cosa_dhcpv6_apis.c cosa_dhcpv4_apis.c ../middle_layer_src/cosa_apis_util.c cosa_dhcpv4_webconfig_apis.c lan_webconfig_param.c macbinding_webconfig_param.c cosa_x_cisco_com_devicecontrol_apis.c
if FEATURE_SUPPORT_MAPT_NAT46
libCcspDhcpMgr_board_sbapi_la_SOURCES += cosa_mapt_apis.c
endif
libCcspDhcpMgr_board_sbapi_la_LDFLAGS = -lccsp_common -lhal_moca -lulog -lsyscfg -lsysevent -ltime_conversion -llmapi -lwebconfig_framework -lmsgpackc -lcm_mgnt -lnet
