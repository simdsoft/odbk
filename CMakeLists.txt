# Tencent is pleased to support the open source community by making ODBK available.
# Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
# Licensed under the MIT License (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
# http://opensource.org/licenses/MIT
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

cmake_minimum_required(VERSION 2.8)

if ( WIN32 AND NOT CYGWIN AND NOT ( CMAKE_SYSTEM_NAME STREQUAL "WindowsStore" ) AND NOT ANDROID)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT" CACHE STRING "")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd" CACHE STRING "")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT" CACHE STRING "")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd" CACHE STRING "")
endif ()

project(odbk)

if ( IOS )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
endif ()

#option ( LUA_COMPAT_MODULE "compat module" ON)

find_path(ODBK_PROJECT_DIR NAMES SConstruct
    PATHS 
    ${CMAKE_SOURCE_DIR}
    NO_DEFAULT_PATH
    )

MARK_AS_ADVANCED(ODBK_PROJECT_DIR)

#set(LUA_SRC_PATH lua-5.3.5/src)
#set(LUAJIT_SRC_PATH luajit-2.1.0b3/src)

#begin yasio
if (NOT WIN32 OR CYGWIN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if (ANDROID)
    if (NOT THIRDPART_LIB)
        set(THIRDPART_LIB "-llog")
    else()
        set(THIRDPART_LIB "${THIRDPART_LIB} -llog")
    endif()
endif()

set (YASIO_SRC
    yasio/yasio/ibstream.cpp
    yasio/yasio/obstream.cpp
    yasio/yasio/xxsocket.cpp
    yasio/yasio/yasio.cpp
    yasio/yasio/impl/lyasio.cpp
    yasio/yasio/impl/yasio-ni.cpp
)
set_property(
    SOURCE ${YASIO_SRC}
    APPEND
    PROPERTY COMPILE_DEFINITIONS
)
list(APPEND THIRDPART_INC  yasio)
set (THIRDPART_SRC ${THIRDPART_SRC} ${YASIO_SRC})
#end yasio

#begin minhook
list(APPEND THIRDPART_INC "minhook/include")
#endf minhook

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${THIRDPART_INC}
)

set ( ODBK_CORE
    odbk/odbk_core.cpp
    odbk/odbk_dbghelp.cpp
    odbk/odbk_entry.cpp
    odbk/odbk_hook.cpp
    odbk/odbk_lhook.cpp
    odbk/odbk_lload.cpp
    odbk/odbk_utils.cpp
    odbk/odbk_proto.cpp
)

macro(source_group_by_dir proj_dir source_files)
    if(MSVC OR APPLE)
        get_filename_component(sgbd_cur_dir ${proj_dir} ABSOLUTE)
        foreach(sgbd_file ${${source_files}})
			get_filename_component(sgbd_abs_file ${sgbd_file} ABSOLUTE)
            file(RELATIVE_PATH sgbd_fpath ${sgbd_cur_dir} ${sgbd_abs_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC OR APPLE)
endmacro(source_group_by_dir)

source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} ODBK_CORE)

if (APPLE)
    if (IOS)
        set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
        add_library(ODBK STATIC
           ${ODBK_CORE}
           ${THIRDPART_SRC}
        )
		set_xcode_property (ODBK IPHONEOS_DEPLOYMENT_TARGET "7.0" "all")
    else ()
        set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_64_BIT)")
        add_library(ODBK MODULE
            ${ODBK_CORE}
            ${THIRDPART_SRC}
        )
        set_target_properties ( ODBK PROPERTIES BUNDLE TRUE )
        #set_target_properties ( ODBK PROPERTIES FRAMEWORK TRUE )
        #set_target_properties ( ODBK PROPERTIES MACOSX_RPATH TRUE )
    endif ()
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Switch")
    add_library(ODBK STATIC
        ${ODBK_CORE}
        ${THIRDPART_SRC}
    )
    target_compile_options(ODBK PRIVATE -m64 -mcpu=cortex-a57+fp+simd+crypto+crc -fno-common -fno-short-enums -ffunction-sections -fdata-sections -fPIC -fms-extensions)
else ( )
    add_library(ODBK SHARED
        ${ODBK_CORE}
        ${THIRDPART_SRC}
    )
endif ( )

if(UINT_ESPECIALLY)
    ADD_DEFINITIONS(-DUINT_ESPECIALLY)
endif()
	
if ( WIN32 AND NOT CYGWIN )
    target_link_libraries(ODBK
        ws2_32
        ${THIRDPART_LIB} 
    )
else ( )
    target_link_libraries(ODBK
        m
        ${THIRDPART_LIB} 
    )
endif ( )
