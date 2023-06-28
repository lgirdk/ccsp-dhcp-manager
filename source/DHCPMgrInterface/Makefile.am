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

AM_CFLAGS = -fno-exceptions -Wall -Wextra
AM_LDFLAGS = -lpthread -lccsp_common -lnanomsg

noinst_LTLIBRARIES = libifl.la

libifl_la_CPPFLAGS = -I$(top_srcdir)/source/DHCPMgrInterface/include

libifl_la_SOURCES = ifl_thread.c ifl_queue_impl.c ifl.c

libifl_la_LIBADD = -lccsp_common -lsysevent -lulog
