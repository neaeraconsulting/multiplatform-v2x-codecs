#!/bin/bash
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

# Run script for Dockerfile

# Echo on
set -x

# Build and test the CLI and shared library
cmake .
cmake --build . --verbose
cat examples/spat.hex | ./convert-v2x uper jer MessageFrame
mkdir out
cp convert-v2x libv2x.so.1.0.0 out/


# Clean up cli build
cmake --build . --target clean
rm -r CMakeFiles
rm cmake_install.cmake
rm CMakeCache.txt
rm Makefile

# Configure with Emscripten toolchain and build wasm
cmake -DCMAKE_TOOLCHAIN_FILE=$EM_CMAKE_TOOLCHAIN -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" .
make VERBOSE=1
$EMCC libv2x.a -o v2x.html -O2 -Wno-unused-main -sMINIFY_HTML=0 -sMODULARIZE=1 -sEXPORT_ES6=1 \
      -sINVOKE_RUN=0 \
      -sEXPORTED_FUNCTIONS=_convert_str,_convert_bytes,_malloc,_free,_memset,_main \
      -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,stringToNewUTF8,UTF8ToString,writeArrayToMemory,getValue

# Save compiled wasm and js files to output
cp v2x.js v2x.wasm out/

# Copy outputs to shared volume
cp /v2x_wasm/out/* /build

# Keep the container running
tail -f /dev/null