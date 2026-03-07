#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_HEX_BYTE_COUNT 16
#define MAX_BIN_BYTE_COUNT 6
#define HEX_LINE_LEN 67
#define BIN_LINE_LEN 71

long byte_limit     = -1;           // -l --len option flag
bool is_bin         = false;        // -b --bin option flag 

void outputHexLine(unsigned int offset, char* bytes) {
    printf("%08x: ", offset);
    short i = 0;

    while (bytes[i] != '\0') {
        printf("%02x", bytes[i++]);
        if (i % 2 == 0) printf(" ");
    }
    i++;
    printf("%*s", HEX_LINE_LEN - 24 - (i << 1) - (i >> 1), ""); // add whitespace

    i = 0;
    while (bytes[i] != '\0') {
        if (bytes[i] == '\n' || bytes[i] == '\t') printf(".");
        else printf("%c", bytes[i]);
        i++;
    }

    printf("\n");
}

void outputBinLine(unsigned int offset, char* bytes) {
    printf("%08x: ", offset);
    short i = 0;
    while (bytes[i] != '\0') {
        for (short j = 7; j >= 0; j--) printf("%d", (bytes[i] >> j) & 1);
        printf(" ");
        i++;
    }
    printf("%*s", BIN_LINE_LEN - 16 - i - (i << 3), ""); // add whitespace

    i = 0;
    while (bytes[i] != '\0') {
        if (bytes[i] == '\n' || bytes[i] == '\t') printf(".");
        else printf("%c", bytes[i]);
        i++;
    }
    printf("\n");
}

void hexDump(FILE *fp) {
    char buff; // buffer for a byte
    short i = 0; // where we are in our hex line
    char bytes[MAX_HEX_BYTE_COUNT + 1] = {0}; // our current hex line
    unsigned int offset = 0x0;

    long total_byte_count = 0;

    while (fread(&buff, sizeof(char), 1, fp) > 0) {
        bytes[i++] = buff;
        total_byte_count++;

        if (byte_limit > 0 && total_byte_count == byte_limit) break; // -l flag limit break
        
        if (!is_bin && i == MAX_HEX_BYTE_COUNT) {
            outputHexLine(offset, bytes);
            i = 0;
            memset(&bytes[0], 0, sizeof(bytes));

            if (offset == UINT_MAX - 0xF) offset = 0x0;
            else offset += 0x10;
        }

        if (is_bin && i == MAX_BIN_BYTE_COUNT) {
            outputBinLine(offset, bytes);
            i = 0;
            memset(&bytes[0], 0, sizeof(bytes));

            if (offset == UINT_MAX - 0xF) offset = 0x0;
            else offset += 0x6;
        }
    }

    if (is_bin) outputBinLine(offset, bytes);
    else outputHexLine(offset, bytes);
}

int main(int argc, char *argv[]) {
    int opt;
    static struct option long_options[] = {
        {"len",     required_argument,  NULL,   'l'},
        {"bin",     no_argument,        NULL,   'b'},
        {0, 0, 0, 0}
    };

    int opt_index = 0;
    while ((opt = getopt_long(argc, argv, "l:b", long_options, &opt_index)) != -1) {
        switch (opt) {

            case 'l':
                errno = 0;
                char *endptr;
                byte_limit = strtol(optarg, &endptr, 0);

                if (errno == ERANGE) {
                    fprintf(stderr, "cxd: len value out of range\n");
                    return 1;
                }
                if (*endptr != '\0') {
                    fprintf(stderr, "cxd: invalid len value: %s\n", optarg);
                    return 1;
                }

                break;
            
            case 'b':
                is_bin = true;
                break;

            default:

                break;
        }
    }

    FILE *fp = fopen(argv[argc - 1], "r");
    if (fp == NULL) { 
        perror(argv[argc - 1]);
        return 1;
    }

    hexDump(fp);

    fclose(fp);
    return 0;
}