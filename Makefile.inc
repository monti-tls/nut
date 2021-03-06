# This file is part of nut.
#
# Copyright (c) 2015, Alexandre Monti
#
# nut is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# nut is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with nut.  If not, see <http://www.gnu.org/licenses/>.

## Compilation options
CXX=g++

CXXFLAGS=-fPIC -Wall -Wextra -std=gnu++11
LDFLAGS=
RELEASE_FLAGS=-O3
DEBUG_FLAGS=-DDEBUG -g

## Directories setup
SOURCE_DIR=src
INCLUDE_DIR=include
BUILD_DIR=build
BIN_DIR=bin

## Products setup
PRODUCT=nut

## Colors definitions
red=`tput setaf 1`
green=`tput setaf 2`
yellow=`tput setaf 3`
blue=`tput setaf 4`
rcol=`tput sgr0`

## Build-specific commands
BINARY=$(BIN_DIR)/$(PRODUCT)

CXXFLAGS+=-I$(INCLUDE_DIR) -DPRODUCT_NAME=\"$(PRODUCT)\"
release?=0
ifeq ($(release),1)
        CXXFLAGS+=$(RELEASE_FLAGS)
else
        CXXFLAGS+=$(DEBUG_FLAGS)
endif
