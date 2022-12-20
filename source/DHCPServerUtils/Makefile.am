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

#bin_PROGRAMS = service_dhcp_server
lib_LTLIBRARIES = libdhcp_server_utils.la

AM_CFLAGS = -fno-exceptions -Os -Wall -Wextra
AM_LDFLAGS =  -lpthread -lz -lccsp_common $(DBUS_LIBS)
libdhcp_server_utils_la_CPPFLAGS = -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/dbus-1.0 \
                       -I${PKG_CONFIG_SYSROOT_DIR}$(libdir)/dbus-1.0/include \
                       -I$(top_srcdir)/source/DHCPServerUtils/include

libdhcp_server_utils_la_SOURCES = util.c dhcp_server_functions.c service_dhcp_server.c dhcpv6_server_functions.c dhcpv4_server_interface.c

libdhcp_server_utils_la_LIBADD = -lccsp_common -ltelemetry_msgsender -lulog -lsyscfg -lsysevent -ltime_conversion -lprint_uptime 

#service_dhcp_LDADD = $(top_builddir)/source/util/utils/libutopiautil.la \
#                     $(top_builddir)/source/sysevent/lib/libsysevent.la \
#                     $(top_builddir)/source/syscfg/lib/libsyscfg.la \
#                     $(top_builddir)/source/util/print_uptime/libprint_uptime.la \
#                    -ltelemetry_msgsender
