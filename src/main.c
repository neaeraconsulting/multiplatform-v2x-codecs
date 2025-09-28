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

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>    /* for atoi(3) */
#include <string.h>    /* for strerror(3) */

#define EX_USAGE    64

#include "../generated/asn_application.h"
#include <stddef.h>
#include "./convert.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define PDU_Type_Ptr    NULL


#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
void usage() {

#ifdef __EMSCRIPTEN__
    emscripten_log(EM_LOG_CONSOLE | EM_LOG_INFO, "Hello from libv2x/main.c");
#else
    printf("\nUsage:\n ./convert-v2x [from-encoding] [to-encoding] [PDU]\n\n");
    printf(" where 'from-encoding' and 'to-encoding' can be 'uper', 'xer', or 'jer'.\n Reads one line of text from stdin\n Accepts UPER as hex encoded text.\n\n");
    printf("Examples:\n\n");
    printf("  Convert a file containing a hex encoded UPER MessageFrame to JER:\n");
    printf("  $ cat data.hex | ./convert-v2x uper jer MessageFrame > data.json\n\n");
    printf("  Convert a file containing a SPAT with no MessageFrame from canonical XER to JER:\n");
    printf("  $ cat data.xml | ./convert-v2x xer jer SPAT > data.json\n\n");
#endif

}


#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
int main(int ac, char *av[]) {

    if (!av[1] || !av[2] || !av[3]) {
        usage();
        exit(EX_USAGE);
    }

    printf("PDU=%s\n", av[3]);
    printf("from=%s\n", av[1]);
    printf("to=%s\n", av[2]);


    char line[2048];
    size_t size;
    if (fgets(line, sizeof(line), stdin) != NULL) {
        // Truncate after CR or LF
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
        }
        printf("input: %s\n", line);
    }

    const size_t out_buf_size = 0xFFFFu;
    char * out_buf = calloc(out_buf_size, sizeof(uint8_t));
    convert_str(av[3], av[1], av[2], line, out_buf, out_buf_size);
    printf("%s\n", out_buf);
    free(out_buf);
}



