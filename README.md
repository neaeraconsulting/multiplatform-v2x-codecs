# Multi-Platform J2735/2024 C Codec: WebAssembly, Windows, Linux

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

Provides a simple converter API in C for the complete J2735 (2024) specification, with libraries and command-line tools compiled for multiple platforms to convert messages between UPER, JER, and XER encodings.  Includes:
* A WebAssembly library that can run in any browser, and a Node.js command line app that can run on any platform that supports Node.js.
* A demo website that uses the WebAssembly library.
* A native CLI tool and library for Windows 11.
* A native CLI tool and library for Linux.

## Folder Contents

* `/` - Top level contains CMake build files, Dockerfiles, and build scripts
  * `/src` - A simple converter API written in C, with CLI interface and conversion functions.
  * `/generated` - A C codec for the [SAE J2735 (2024) V2X message set](https://new.sae.org/standards/j2735_202409-v2x-communications-message-set-dictionary) generated from the ASN.1 specification by the open source [asn1c compiler](https://github.com/usdot-fhwa-stol/usdot-asn1c).
  * `/custom` - Edited overrides for the generated files.
  * `/build` - Executable files
    * `v2x.wasm` - The WebAssembly binary.
    * `v2x.js` - Javascript wrapper for the WebAssembly binary.
    * `app.js` - A Node.js command line app that uses the WebAssembly library.
    * `convert-v2x.exe` - Command line tool for Windows.
    * `v2x.dll` - Shared library for Windows.
    * `convert-v2x`,  - Command line tool for Linux.
    * `libv2x.so.1.0.0` - Shared library for Linux.
  * `/site` - Demo website that uses the WebAssembly library.

## Quickstart

### Live Demo

Try the live demo here:

https://salmon-grass-077f66e1e.6.azurestaticapps.net/

The converter tool here also uses the WebAssembly library:

https://cvtools.neaeraconsulting.com/j2735-codec

### Run WebAssembly Command Line

The WebAssembly app can be run on the command line using node.js.  This should work on any platform that supports Node.  Tested in Windows 11 Powershell and Ubuntu WSL.

Prerequisites:
* [npm and Node.js](https://docs.npmjs.com/downloading-and-installing-node-js-and-npm)

```bash
cd build
npm install

# Read SPAT hex from stdin
cat ../examples/spat.hex | npx run-func app.js main "uper" "xer" "MessageFrame"

# Pass SSM hex as argument
npx run-func app.js convert "uper" "jer" "MessageFrame" "001e1562b3aec8bf060000629008125881c628047a004140"

# Convert SSM XML to UPER hex
npx run-func app.js convert "xer" "uper" "MessageFrame" "<MessageFrame><messageId>30</messageId><value><SignalStatusMessage><timeStamp>177070</timeStamp><second>51391</second><sequenceNumber>3</sequenceNumber><status><SignalStatus><sequenceNumber>0</sequenceNumber><id><id>6308</id></id><sigStatus><SignalStatusPackage><requester><id><entityID>9620718A</entityID></id><request>1</request><sequenceNumber>15</sequenceNumber><role><transit/></role></requester><inboundOn><lane>16</lane></inboundOn><status><rejected/></status></SignalStatusPackage></sigStatus></SignalStatus></status></SignalStatusMessage></value></MessageFrame>"
```

### Run Linux Command Line

Requires Linux.

```bash
cd build
cat ../examples/spat.hex | ./convert-v2x uper jer MessageFrame
```

### Run Windows Command Line

Requires Windows.

Use Powershell

```powershell
cd build
cat ..\examples\spat.hex | .\convert-v2x.exe uper jer MessageFrame
```

### Run Demo Site Locally

To run the demo locally, clone this repository, then run `/site` in a web server. Opening the `index.html` file directly in a browser does not work.  Instead, run the static site locally using an IDE or in nginx using Docker.

To run the pre-compiled static website using an IDE:
* From Jetbrains CLion or WebStorm, right-click on `site/index.html` and select "Run".
* From VScode, right-click on `site/index.html`, and select "Open with live server"

Or use Docker to deploy in nginx:

Prerequisites:
* [Docker](https://docs.docker.com/desktop/)

```bash
docker compose up --build -d
```
and point a browser to:

```
http://localhost:8789/index.html
```

## Library API

The WebAssembly and native libraries present the same API.  They allow JavaScript in the browser and native C code to call the same functions to convert J2735 messages between UPER, XER, and JER encodings.

The API functions are:

### convert_str

```c++
function
  size_t convert_str(
  const char *pdu_name,
  const char *from_encoding,
  const char *to_encoding,
  const char *ibuf,
  char *buf,
  size_t max_buf_len);
```

Converts a string representation of a J2735 PDU from one encoding to another.

**Arguments:**

`pdu_name` 
: String with the J2375 PDU, e.g., "MessageFrame", "BasicSafetyMessage".

`from_encoding` 
: String with the name of the encoding of the input ("JER", "XER", or "UPER").

`to_encoding` 
: Target encoding for the output string ("JER", "XER", or "UPER").

`ibuf`
: The input string in UPER hex, XER, or JER format.

`buf` 
: (output) The buffer to store the converted output string.

`max_buf_len` 
: The maximum length of the output buffer.

**Returns:** 
: The length of the converted output string.

### convert_bytes

```c++
size_t convert_bytes(
    const char * pdu_name,
    const char * from_encoding,
    const char * to_encoding,
    const uint8_t * ibuf,
    size_t ibuf_len,
    uint8_t * obuf,
    size_t max_obuf_len);
```
**Arguments:**

Convert a byte array representation of a J2735 PDU from one encoding to another.
UPER format is rww bytes, not hex.
JER and XER are 8-bit text.

`pdu_name` 
: String with the J2375 PDU, e.g., "MessageFrame", "BasicSafetyMessage".

`from_encoding` 
: String with the name of the encoding of the input ("JER", "XER", or "UPER").

`to_encoding` 
: Target encoding for the output ("JER", "XER", or "UPER").

`ibuf` 
: The input byte array in raw UPER, or XER or JER text format.

`ibuf_len` 
: The length of the input byte array.

`obuf` 
: The buffer to store the output byte array.

`max_obuf_len` 
: The maximum length of the output buffer.

**Returns:**
: The length of the converted output byte array.


### C usage example

```C++
int main(int ac, char *av[]) {
    char *line = NULL;
    size_t size;
    if (getline(&line, &size, stdin) == -1) {
        printf("Error reading stdin, no line received.\n");
    }
    const size_t out_buf_size = 0xFFFFu;
    char * out_buf = malloc(out_buf_size);
    convert_str(av[3], av[1], av[2], line, out_buf, out_buf_size);
    printf("%s\n", out_buf);
    free(out_buf);
}
```

### JavaScript usage example

```javascript
function convert(fromValue, toValue, pduValue, inputValue) {
    try {
        const pduPtr = Module.stringToNewUTF8(pduValue);
        const hexPtr = Module.stringToNewUTF8(inputValue);
        const toEncPtr = Module.stringToNewUTF8(toValue);
        const fromEncPtr = Module.stringToNewUTF8(fromValue);
        const bufSize = 65536;
        const bufPtr = Module._malloc(bufSize);
        Module._memset(bufPtr, 0, bufSize);
        const encSize = Module._convert_str(pduPtr, fromEncPtr, toEncPtr, hexPtr, bufPtr, bufSize);
        const resultValue = Module.UTF8ToString(bufPtr);
        Module._free(pduPtr);
        Module._free(hexPtr);
        Module._free(toEncPtr);
        Module._free(fromEncPtr);
        Module._free(bufPtr);
        console.log("Converted to " + encSize + " bytes of " + toValue?.toUpperCase());
        return resultValue;
    } catch (e) {
        console.error(e);
        return '';
    }
}
```
---

# Development

The CLI tool and WebAssembly library can be re-built using the Dockerfile and scripts or manually as described below.

## Build with Docker

### Prerequisites

* Docker

### Build

```bash
docker compose -f docker-compose-wasm.yml up --build -d
```

Allow the container to run and execute the run script.  Eventually it copies the executable files to the `build` volume and leaves the container running.

Run the following command from the host to copy files built by the container from the shared volume to the `site` directory (Don't copy the generated html file, because the site directory contains customized html.)

```bash
cp build/v2x.js build/v2x.wasm site
```

## Manual Build

### Prerequisites

* WSL (Ubuntu) command line
* Python
* CMake
* Git
* Emscripten

### Native executable

To build a command line tool.  

The emscripten build is fairly slow in WSL, so it is useful build a normal executable CLI with the clang compiler to check for any issues first.

#### Install clang and cmake
```bash
sudo apt install clang
sudo apt install cmake
```

#### Build:

```bash
export CC=/usr/bin/clang
cmake .
cmake --build .
```

#### Verbose make:

```bash
cmake .
cmake --build . --verbose
```

#### Clean:

```bash
cmake --build . --target clean
```

#### Run CLI

```bash
cat examples/spat.hex | ./convert-v2x uper jer MessageFrame
```

### WebAssembly

To re-create the WebAssembly file and associated Javascript file:

#### Install Emscripten

Follow instructions:
https://emscripten.org/docs/getting_started/downloads.html

```bash
# Get the emsdk repo
git clone https://github.com/emscripten-core/emsdk.git

# Enter that directory
cd emsdk

# Fetch the latest version of the emsdk (not needed the first time you clone)
git pull

# Download and install the latest SDK tools.
./emsdk install latest

# Make the "latest" SDK "active" for the current user. (writes .emscripten file)
./emsdk activate latest

# Activate PATH and other environment variables in the current terminal
source ./emsdk_env.sh
```

#### Build WebAssembly

From the root directory of the project:

```bash

# Clean
cmake --build . --target clean
# and Manually delete folder "CMakeFiles", and files cmake_install.cmake, CMakeCache.txt, and Makefile

# Configure with Emscripten toolchain
# This is an example. Substitute the actual path to Emscripten.cmake on your machine.
cmake -DCMAKE_TOOLCHAIN_FILE=/mnt/c/Users/ivan/asn1/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" .

# Make verbosely.  Verify it uses emcc as compiler
# This step is pretty slow, takes a couple hours to complete.
make VERBOSE=1

# Compile to wasm, js and html
#
# Module options:
# Use -sMODULARIZE to generate a modularized Javascript wrapper.
# Use -sEXPORT_ES6 to use ES module system instead of the default commonJS modules
# Use -sMINIFY_HTML=0 to make HTML readable
# JS module ref: https://emscripten.org/docs/compiling/Modularized-Output.html
emcc libv2x.a -o v2x.html -O2 -Wno-unused-main -sMINIFY_HTML=0 -sMODULARIZE=1 -sEXPORT_ES6=1 -sEXPORTED_FUNCTIONS=_convert_str,_malloc,_free,_memset -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,stringToNewUTF8,UTF8ToString

# Copy built files to site directory
# (Don't copy the generated html file, because the site directory contains customized html.)
cp v2x.js v2x.wasm site

```

## Windows Build

The Windows exe and dll were built using the same source code Visual Studio 2022 with the Clang toolset and CMake, with the settings file `CMakeSettings.json`.

## Known Issues and Future Plans

### JSON object element order

The JER dialect implemented by the asn1c codec requires that elements of a SEQUENCE be provided in the same order as the tags in the ASN.1 specification.  This is a limitation of the existing open source asn1c compiler, but is not required by the JER standard, which follows the JSON standard and allows elements to be in any order. 

### Java Language Integration

The simple C API header in `src/convert.h` will facilitate integration with the Java language using the Foreign Function and Memory (FFM) API using the fextract tool to generate Java bindings.  This is planned for a future release.

## Contributing
We welcome contributions to this open source project! Whether you want to report bugs, suggest new features, improve documentation, or submit code, your input is appreciated.

To report an issue:

1. First check this README and all other documentation in this repo, and existing issues, to see if your problem or suggestion has already been addressed.
2. Create a new issue here: https://github.com/neaeraconsulting/j2735-ffm-java/issues
3. Provide a clear and concise description of the issue or feature request, including details of the development environment, platform, operating system, or browser used, and list specific and detailed steps to reproduce the issue.

To contribute:

1. Fork the repository and create a new branch for your changes.
2. Make your changes, following the existing code style.
3. Submit a pull request with a clear description of your changes and the problem they solve.

Please ensure your contributions comply with the project's license and include appropriate tests or examples where applicable.

Thank you for helping improve this project!

## License

All code in this repository is licensed under the Apache 2.0 License, except for skeleton files in the `/generated` directory from the `asn1c` project with header comments indicating the BSD license.

---

Copyright &copy; 2025 Neaera Consulting, LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

---

Copyright (c) 2003-2017  Lev Walkin <vlm@lionet.info> and contributors.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.