#  Copyright (C) 2021 Xcalibyte (Shenzhen) Limited.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

cmake_minimum_required(VERSION 3.5)

if (NOT CLANG_HOME)
  set (CLANG_HOME "$ENV{CLANG_HOME}")
endif ()

if (NOT CLANG_HOME)
  message (FATAL_ERROR "CLANG_HOME isn't set. Use -DCLANG_HOME=<path_too_clang> to specify clang prebuilt files.")
else ()
  message ("-- Check for CLANG_HOME: ${CLANG_HOME}")
endif ()

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug)
endif ()

set (CMAKE_CXX_STANDARD 14)

set (XSCA_SRCS
  cc1_main.cpp
  driver.cpp
  xsca_gen.cpp
  xsca_link.cpp
  )

set (XSCA_INCS
  .
  include
  )

add_definitions (-DBUILD_XSCA)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions (-DDEBUG -DIs_True_On)
  set (OPT_FLAGS "-O0")
else ()
  add_definitions (-DNDEBUG)
  set (OPT_FLAGS "-O2")
endif ()

set (LANG_CXX_FLAGS "-fno-exceptions -fno-rtti -pthread -no-pie")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPT_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LANG_CXX_FLAGS} ${OPT_FLAGS}")

include_directories (
  ${XSCA_INCS}
  ${CLANG_HOME}/include
  )

link_directories (
  ${CLANG_HOME}/lib
  )

# core files
add_subdirectory (core)
list (APPEND XCSA_LIBS xscaCore)

# rule files
add_subdirectory (rules)
list (APPEND XCSA_LIBS xscaRule)

set (XSCA_CLANG_LIBS
  PRIVATE
  -Wl,--start-group
  clangAnalysis
  clangAST
  clangBasic
  clangDriver
  clangEdit
  clangFrontend
  clangFrontendTool
  clangLex
  clangParse
  clangRewrite
  clangRewriteFrontend
  clangSema
  clangSerialization
  clangTooling
  LLVMBinaryFormat
  LLVMBitReader
  LLVMBitstreamReader
  LLVMCore
  LLVMDemangle
  LLVMFrontendOpenMP
  LLVMMC
  LLVMMCParser
  LLVMOption
  LLVMProfileData
  LLVMRemarks
  LLVMSupport
  -Wl,--end-group
  m
  dl
  rt
  pthread
  )

# make final target
add_executable(xsca
  ${XSCA_SRCS}
  )

target_link_libraries (xsca
  PRIVATE
  ${XCSA_LIBS}
  ${XSCA_CLANG_LIBS}
  )

