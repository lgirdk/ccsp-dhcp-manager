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
AM_CFLAGS += -D_ANSC_LITTLE_ENDIAN_
AM_LDFLAGS = -lccsp_common
AM_LDFLAGS = -ltime_conversion -lhal_plaform -lsysevent -lutapi -lhal_moca -lutctx -lulog -llmapi
AM_LDFLAGS = -lwebconfig_framework
AM_LDFLAGS = -lmsgpackc

ACLOCAL_AMFLAGS = -I m4
hardware_platform = i686-linux-gnu

noinst_LTLIBRARIES = libCcspDhcpMgr_board_sbapi.la
libCcspDhcpMgr_board_sbapi_la_CPPFLAGS = -I$(top_srcdir)/source/TR-181/include -I$(top_srcdir)/source/TR-181/middle_layer_src -I$(top_srcdir)/source/DHCPServerUtils/include  -I$(top_srcdir)/source/DHCPClientUtils

libCcspDhcpMgr_board_sbapi_la_SOURCES = cosa_dhcpv6_apis.c cosa_dhcpv4_apis.c ../middle_layer_src/cosa_apis_util.c cosa_common_util.c cosa_dns_apis.c service_dhcpv6_client.c helpers.c cosa_dhcpv4_webconfig_apis.c lan_webconfig_param.c macbinding_webconfig_param.c cosa_x_cisco_com_devicecontrol_apis.c cosa_x_cisco_com_devicecontrol_apis.c service_dhcpv4_client.c
if FEATURE_SUPPORT_MAPT_NAT46
libCcspDhcpMgr_board_sbapi_la_SOURCES += cosa_mapt_apis.c
endif
libCcspDhcpMgr_board_sbapi_la_LDFLAGS = -lccsp_common -lhal_moca -lulog -lsyscfg -lsysevent -ltime_conversion -llmapi -lwebconfig_framework -lmsgpackc -lcm_mgnt
