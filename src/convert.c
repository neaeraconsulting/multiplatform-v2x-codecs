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
#include "convert.h"
#include "../generated/asn_application.h"
#include <stdlib.h>    /* for atoi(3) */
#include <string.h>    /* for strerror(3) */


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define PDU_Type_Ptr    NULL

#ifdef _WIN64
const void* nullptr = NULL;
#endif

extern asn_TYPE_descriptor_t *asn_pdu_collection[];

static void hex_to_bin(const char *hex, size_t hex_len, uint8_t *bytes) {
    size_t bin_len = hex_len / 2;
    for (unsigned int i = 0, j = 0; i < bin_len; i++, j+=2) {
        bytes[i] = (hex[j] % 32 + 9) % 25 * 16 + (hex[j+1] % 32 + 9) % 25;
    }
}

static void bin_to_hex(const uint8_t *bytes, size_t bytes_len, char *hex) {
    if (!bytes) {
        fprintf(stderr, "Null byte array passed to bin_to_hex\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i < bytes_len; i++) {
        hex += sprintf(hex, "%02x", bytes[i]);
    }
}

static enum asn_transfer_syntax abbrev_to_syntax(const char * abbrev) {
    if (strcmp("xer", abbrev) == 0) {
        return ATS_CANONICAL_XER;
    }
    if (strcmp("jer", abbrev) == 0) {
        return ATS_JER_MINIFIED;
    }
    if (strcmp("uper", abbrev) == 0) {
        return ATS_UNALIGNED_BASIC_PER;
    }
    fprintf(stderr, "Unknown encoding: %s  Expect 'xer', 'jer', or 'uper'.\n", abbrev);
    exit(EXIT_FAILURE);
}



#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
size_t convert_bytes(const char * pdu_name,
    const char * from_encoding,
    const char * to_encoding,
    const uint8_t * ibuf,
    size_t ibuf_len,
    uint8_t * obuf,
    size_t max_obuf_len) {

    asn_TYPE_descriptor_t *pduType = PDU_Type_Ptr;

    asn_TYPE_descriptor_t **pdu = asn_pdu_collection;
    while(*pdu && strcmp((*pdu)->name, pdu_name)) pdu++;
    if(*pdu) {
        pduType = *pdu;
    } else {
        fprintf(stderr, "%s: Unrecognized PDU.\n", pdu_name);
        exit(EXIT_FAILURE);
    }

    enum asn_transfer_syntax osyntax = abbrev_to_syntax(to_encoding);
    enum asn_transfer_syntax isyntax = abbrev_to_syntax(from_encoding);

    const asn_codec_ctx_t *opt_codec_ctx = nullptr;
    void *structure = nullptr;

    // Decode
    asn_dec_rval_t rval = asn_decode(opt_codec_ctx, isyntax, pduType, &structure, ibuf, ibuf_len);

    if (rval.code != RC_OK) {
        fprintf(stderr, "%s: Error decoding PDU\n", pduType->name);
        ASN_STRUCT_FREE(*pduType, structure);
        exit(EXIT_FAILURE);
    }

    // Check constraints
    char errbuff[256];
    size_t errlen = sizeof(errbuff);
    int constraint_result = asn_check_constraints(pduType, structure, errbuff, &errlen);
    if (constraint_result != 0) {
        fprintf(stderr, "Decoding was successful, but constraint check failed, can't re-encode: %s\n", errbuff);
        ASN_STRUCT_FREE(*pduType, structure);
        exit(EXIT_FAILURE);
    }

    // Encode
    asn_encode_to_new_buffer_result_t enc_result = {NULL, 0, nullptr};
    enc_result = asn_encode_to_new_buffer(opt_codec_ctx, osyntax, pduType, structure);
    if (!enc_result.buffer) {
        fprintf(stderr, "Error encoding to %d\n", osyntax);
        ASN_STRUCT_FREE(*pduType, structure);
        exit(EXIT_FAILURE);
    }
    ASN_STRUCT_FREE(*pduType, structure);

    const size_t num_encoded_bytes = enc_result.result.encoded;

    if (num_encoded_bytes > max_obuf_len) {
        memcpy(obuf, enc_result.buffer, max_obuf_len);
        fprintf(stderr, "Warning, truncating output.  Max buffer size %ld is too small\n", max_obuf_len);
    } else {
        memcpy(obuf, enc_result.buffer, num_encoded_bytes);
    }

    free(enc_result.buffer);
    return num_encoded_bytes;

}


#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
size_t convert_str(const char * pdu_name,
    const char * from_encoding,
    const char * to_encoding,
    const char * ibuf,
    char * buf,
    const size_t max_buf_len) {

    const size_t len = strlen(ibuf);

    enum asn_transfer_syntax osyntax = abbrev_to_syntax(to_encoding);
    int is_to_uper = (osyntax == ATS_UNALIGNED_BASIC_PER);

    enum asn_transfer_syntax isyntax = abbrev_to_syntax(from_encoding);
    int is_from_uper = (isyntax == ATS_UNALIGNED_BASIC_PER);

    size_t num_encoded_bytes;

    uint8_t* obuf = malloc(max_buf_len);

    // If input is UPER, convert from hex string to byte array
    if (is_from_uper) {
        size_t input_bytes_len = len / 2;
        unsigned char bytes[input_bytes_len];
        hex_to_bin(ibuf, len, bytes);
        num_encoded_bytes = convert_bytes(pdu_name, from_encoding, to_encoding,
            bytes, input_bytes_len, obuf, max_buf_len);
    } else {
        num_encoded_bytes = convert_bytes(pdu_name, from_encoding, to_encoding,
            (const uint8_t*)ibuf, len, obuf, max_buf_len);
    }


    // If output is UPER, convert to hex string
    if (is_to_uper) {
        // Convert UPER result to hex
        size_t hex_result_len = num_encoded_bytes * 2;
        char hex_result[hex_result_len];
        bin_to_hex(obuf, num_encoded_bytes, hex_result);
        if (hex_result_len > max_buf_len) {
            strncpy(buf, hex_result, max_buf_len);
            fprintf(stderr, "Warning truncating hex UPER output.  Max buffer size %ld is too small\n", max_buf_len);
        } else {
            strncpy(buf, hex_result, hex_result_len);
        }
    } else {
        if (num_encoded_bytes > max_buf_len) {
            strncpy(buf, (const char *)obuf, max_buf_len);
            fprintf(stderr, "Warning, truncating output.  Max buffer size %ld is too small\n", max_buf_len);
        } else {
            strncpy(buf, (const char *)obuf, num_encoded_bytes);
        }
    }

    free(obuf);

    return num_encoded_bytes;
}


