# This makefile creates a build tree, always 
# required to be outside of source tree

# this Makefile was inspired, and some parts stolen from the 
# kernel makefile of L4 from Karlsruhe; copyright at the end

ifeq ($(strip $(SRCDIR)), )
#SRCDIR not defined, we must be in the source tree
ifeq ($(strip $(BUILDDIR)), )
all: usage
else
all: create_build
endif
else
# we are in the build tree
all: ebbtest
endif

usage:
	@echo "usage:"
	@echo "1. Run make BUILDDIR=<absolute path of nonexisting directory>"
	@echo "\t e.g.: make BUILDDIR=/Users/fred/SESA/build-amd64"
	@echo "2. cd to the build directory"
	@echo "3. modify the settings in Makeconf.local"
	@echo "4. run make in build directory"
	@exit 0

create_build::
	@echo ""
	@#
	@# Bail out if BUILDDIR is not an absolute path
	@#
	@if [ "/" != "`echo $(BUILDDIR) | cut -c1`" ]; then \
	   echo "Oops, $(BUILDDIR) is not an absolute path ... aborting." && \
	   echo ""; \
	   exit -1 ; \
	 fi
	@#
	@# Check, if we try to mess up an existing directory
	@#
	@if [ -d $(BUILDDIR) ]; then \
	   echo "Oops, $(BUILDDIR) already exists ... aborting." && \
	   echo ""; \
	   exit -1 ; \
	 fi
	@echo "Setting up build directory $(BUILDDIR) ..."
	@#
	@# Check if the parent of BUILDDIR exists.  If not, create it
	@#
	@if [ ! -d $(dir $(BUILDDIR:%/=%)) ]; then \
	   mkdir -p $(dir $(BUILDDIR:%/=%)); \
	 fi
	@#
	@# create all the libraries build directories
	@#
	@mkdir $(BUILDDIR)
	@echo "KLUDGE copying libixp-0.5 to build side"
	@cp -r libixp-0.5 $(BUILDDIR)/
	@echo "creating Makeconf.local in remote"
	@cp config/Makeconf.local.template $(BUILDDIR)/Makeconf.local
	@echo "modifying SRCDIR to point back here"
	@echo "SRCDIR=$(CURDIR)" >> $(BUILDDIR)/Makeconf.local
	@echo "creating new Makefile that will include this one"
	@cp config/Makefile.template $(BUILDDIR)/Makefile

# this include must come at end, since any double colon rules
# must execute in subdirs after the ones in this directory makefile
ifeq ($(strip $(SRCDIR)), )
#SRCDIR not defined, we must be in the source tree
-include ebb/ebbtest.mk
else
-include $(SRCDIR)/ebb/ebbtest.mk
endif

######################################################################
#                
# copyright (C) 2001-2003,  Karlsruhe University
#                
# File path:     Makefile
# Description:   Master Makefile for pistachio
#                
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#                
# $Id: Makefile,v 1.15 2007/03/12 21:19:13 skoglund Exp $
#                
######################################################################
