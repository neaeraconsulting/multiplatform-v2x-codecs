#   Copyright 2025 Neaera Consulting LLC
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

cmake_minimum_required(VERSION 3.28)

# soversion.  bump this to indicate breaking changes in the shared library
set(LIB_SO_VERSION 1)

project(v2x_wasm LANGUAGES C VERSION ${LIB_SO_VERSION}.0.0 DESCRIPTION "v2x cli and webassembly")


set(CMAKE_C_STANDARD 23)

if (${CMAKE_TOOLCHAIN_FILE} MATCHES ".+Emscripten.+")
    set(CMAKE_C_FLAGS "-DASN_PDU_COLLECTION -Wno-parentheses-equality")
    set(CMAKE_C_FLAGS_RELEASE "-O2")
else()
    set(CMAKE_C_FLAGS "-DASN_PDU_COLLECTION -Wno-parentheses-equality -O2")
endif()


file (GLOB generated_SRC
        "generated/*.h"
        "generated/*.c"
)

file (GLOB custom_SRC
    "custom/*.c")

if (${CMAKE_TOOLCHAIN_FILE} MATCHES ".+Emscripten.+")

    # Library for WebAssembly
    add_library(v2x
            ${custom_SRC}
            ${generated_SRC}
            src/convert.h
            src/convert.c
            src/main.c
    )

    target_include_directories(v2x PUBLIC generated)
    target_include_directories(v2x PUBLIC custom)
    set_target_properties(v2x PROPERTIES VERSION ${PROJECT_VERSION})

else()

    # Native shared library
    add_library(v2x SHARED
            ${custom_SRC}
            ${generated_SRC}
            src/convert.h
            src/convert.c
    )

    target_include_directories(v2x PUBLIC generated)
    target_include_directories(v2x PUBLIC custom)
    set_target_properties(v2x PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${LIB_SO_VERSION})

    # -fPIC option to create library suitable for jextract
    set_property(TARGET v2x PROPERTY POSITION_INDEPENDENT_CODE ON)

    # Standalone CLI Executable
    add_executable(convert-v2x
            ${custom_SRC}
            ${generated_SRC}
            src/convert.h
            src/convert.c
            src/main.c
    )

    target_include_directories(convert-v2x PUBLIC generated)
    target_include_directories(convert-v2x PUBLIC custom)
endif()






