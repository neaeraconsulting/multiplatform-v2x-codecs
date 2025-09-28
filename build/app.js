//   Copyright 2025 Neaera Consulting LLC
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

import initModule from "./v2x.js";
import readline from 'readline';

// fromHex and toHex are new in browsers and don't exist in Node in 2025
// polyfill them to be sure they exist for node test
import fromHex from 'es-arraybuffer-base64/Uint8Array.fromHex';
import toHex from 'es-arraybuffer-base64/Uint8Array.prototype.toHex';
import fromBase64 from 'es-arraybuffer-base64/Uint8Array.fromBase64';
import toBase64 from 'es-arraybuffer-base64/Uint8Array.prototype.toBase64';


let Module = {
    print(...args) {
        console.log(...args);
    },
    printErr(...args) {
        console.error(...args);
    },
    setStatus(text) {
        console.log('module status: ' + text);
    },
    totalDependencies: 0,
    monitorRunDependencies(left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);
        Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies - left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
    },
    // Placeholder functions to be replaced on initialization so that typscript in the IDE is happy
    stringToNewUTF8: (x) => {},
    _malloc: (x) => {},
    _free: (x) => {},
    _memset: (x, y, z) => {},
    _convert_str: (x, y, z, a, b, c) => {},
    _convert_bytes: (x, y, z, a, b, c, d) => {},
    UTF8ToString: (x) => {},
    callMain: (argc, argv) => {},
    cwrap: (ident, returnType, argTypes) => {},
    writeArrayToMemory: (array, buffer) => {},
    getValue: (ptr, type) => {}
};

const init = async () => {
    await initModule(Module);
}

await init();




// Accept input from stdin for testing
function main(fromValue, toValue, pduValue) {

    try {

        const rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout,
            terminal: false
        });

        rl.on('line', (line) => {
            const result = convert(fromValue, toValue, pduValue, line);
            console.log(result);
        });

        rl.once('close', () => {
        });
    } catch (e) {
        console.error(0);
    }
}

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

// For testing byte array method on the command line
function convertBase64(fromValue, toValue, pduValue, base64) {
    try {
        let byteArray;
        if (fromValue === 'uper') {
            byteArray = fromBase64(base64);
        } else {
            byteArray = Buffer.from(base64, 'utf-8');
        }
        const result = convertBytes(fromValue, toValue, pduValue, byteArray);
        if (result instanceof Uint8Array) {
            return toBase64(result);
        } else {
            return result;
        }
    } catch (e) {
        console.error(e);
        return '';
    }
}

// bytes: Uint8Array
function convertBytes(fromValue, toValue, pduValue, byteArray) {
    try {
        const pduPtr = Module.stringToNewUTF8(pduValue);
        const toEncPtr = Module.stringToNewUTF8(toValue);
        const fromEncPtr = Module.stringToNewUTF8(fromValue);

        // Input buffer
        const inBufSize = byteArray.byteLength;
        const inPtr = Module._malloc(byteArray.byteLength);
        Module.writeArrayToMemory(byteArray, inPtr);


        // Output buffer
        const bufSize = 65536;
        const bufPtr = Module._malloc(bufSize);
        Module._memset(bufPtr, 0, bufSize);
        const encSize = Module._convert_bytes(pduPtr, fromEncPtr, toEncPtr, inPtr, inBufSize, bufPtr, bufSize);

        if (toValue === 'uper') {
            let resultArray = [];
            for (let i = 0; i < encSize; i++) {
                resultArray.push(Module.getValue(bufPtr + i, 'i8'));
            }
            Module._free(pduPtr);
            Module._free(toEncPtr);
            Module._free(fromEncPtr);
            Module._free(bufPtr);
            return new Uint8Array(resultArray);
        } else {
            const resultValue = Module.UTF8ToString(bufPtr);
            Module._free(pduPtr);
            Module._free(toEncPtr);
            Module._free(fromEncPtr);
            Module._free(bufPtr);
            console.log("Converted to " + encSize + " bytes of " + toValue?.toUpperCase());
            return resultValue;
        }
    } catch (e) {
        console.error(e);
        return '';
    }
}



export { main, convert, convertBytes, convertBase64 };






