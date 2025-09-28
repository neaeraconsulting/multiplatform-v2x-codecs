/*
   Copyright 2025 Neaera Consulting LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#ifndef _CONVERT_H_
#define _CONVERT_H_

// For size_t
#include <stddef.h>
#include <stdint.h>


/**
 Convert a byte array representation of a J2735 PDU from one encoding to another.
 UPER format is rww bytes, not hex.
 JER and XER are 8-bit text.

 @param pdu_name String with the J2375 PDU, e.g., "MessageFrame", "BasicSafetyMessage".
 @param from_encoding String with the name of the encoding of the input ("JER", "XER", or "UPER").
 @param to_encoding Target encoding for the output ("JER", "XER", or "UPER").
 @param ibuf The input byte array in raw UPER, or XER or JER text format.
 @param ibuf_len The length of the input byte array.
 @param obuf The buffer to store the output byte array.
 @param max_obuf_len The maximum length of the output buffer.
 @return The length of the converted output byte array.
*/
size_t convert_bytes(
    const char * pdu_name,
    const char * from_encoding,
    const char * to_encoding,
    const uint8_t * ibuf,
    size_t ibuf_len,
    uint8_t * obuf,
    size_t max_obuf_len);


/**
 Convert a string representation of a J2735 PDU from one encoding to another.
 UPER encoding is input and returned as hex strings.

 @param pdu_name String with the J2375 PDU, e.g., "MessageFrame", "BasicSafetyMessage".
 @param from_encoding String with the name of the encoding of the input ("JER", "XER", or "UPER").
 @param to_encoding Target encoding for the output string ("JER", "XER", or "UPER").
 @param ibuf The input string in UPER hex, XER, or JER format.
 @param buf The buffer to store the converted output string.
 @param max_buf_len The maximum length of the output buffer.
 @return The length of the converted output string.
*/
size_t convert_str(
    const char * pdu_name,
    const char * from_encoding,
    const char * to_encoding,
    const char * ibuf,
    char * buf,
    size_t max_buf_len);


#endif