#
# Copyright (c) 2023-present, Trail of Bits, Inc.
# All rights reserved.
#
# This source code is licensed in accordance with the terms specified in
# the LICENSE file found in the root directory of this source tree.
#

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")
set(CMAKE_EXPORT_COMPILE_COMMANDS true CACHE BOOL "Generate the compile_commands.json file (forced)" FORCE)

set(PILLAR_INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}" CACHE PATH "Directory in which pillar libraries will be installed")
set(PILLAR_INSTALL_BIN_DIR "${CMAKE_INSTALL_BINDIR}" CACHE PATH "Directory in which pillar binaries will be installed")
set(PILLAR_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/pillar" CACHE PATH "Directory in which pillar headers will be installed")
set(PILLAR_INSTALL_SHARE_DIR "${CMAKE_INSTALL_DATADIR}" CACHE PATH "Directory in which pillar CMake files will be installed")

option(PILLAR_ENABLE_INSTALL "Set to ON to enable the install target" ON)
