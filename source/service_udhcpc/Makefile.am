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

bin_PROGRAMS = udhcpc_response_handler

AM_CFLAGS = -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/dbus-1.0 -I${PKG_CONFIG_SYSROOT_DIR}$(libdir)/dbus-1.0/include

AM_LDFLAGS =  -lpthread -lz

udhcpc_response_handler_SOURCES = service_udhcpc.c

udhcpc_response_handler_LDADD =  -ltelemetry_msgsender -lulog -lsyscfg -lsysevent -ltime_conversion

#udhcpc_response_handler_LDADD = $(top_builddir)/source/util/utils/libutopiautil.la \
#                    $(top_builddir)/source/services/lib/libsrvmgr.la \
#                    $(top_builddir)/source/sysevent/lib/libsysevent.la \
#                    $(top_builddir)/source/syscfg/lib/libsyscfg.la \
#                    $(top_builddir)/source/ulog/libulog.la \
#                    $(top_builddir)/source/util/print_uptime/libprint_uptime.la \
#                    -ltelemetry_msgsender

