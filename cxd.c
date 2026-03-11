#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_HEX_BYTE_COUNT 16
#define MAX_BIN_BYTE_COUNT 6
#define DEFAULT_HEX_BYTES_PER_GROUP 2
#define DEFAULT_BIN_BYTES_PER_GROUP 1
#define HEX_LINE_LEN 67
#define BIN_LINE_LEN 71

long    byte_limit              = -1;         // -l --len option flag
bool    is_bin                  = false;      // -b --bin option flag 
char    bytes_per_group         = -1;         // -g       option flag (-1 means default)

void printByteGroup(char* bytes, short* i) {
    int byte_limit = DEFAULT_HEX_BYTES_PER_GROUP;
    if (bytes_per_group > 0) {
        byte_limit = bytes_per_group;
    }

    int byte_count = 0;
    while (bytes[*i] != '\0' && byte_count < byte_limit) {
        byte_count++;

        if (is_bin) for (short j = 7; j >= 0; j--) printf("%d", (bytes[*i] >> j) & 1);
        else printf("%02x", bytes[*i]);

        *i += 1;
    }
}

void outputLine(unsigned int offset, char* bytes) {
    printf("%08x: ", offset);
    short i = 0;

    while (bytes[i] != '\0') {
        printByteGroup(bytes, &i);
        printf(" ");
    }
    if (is_bin) {
        printf("%*s", BIN_LINE_LEN - 16 - i - (i << 3), ""); // add whitespace
    } else {
        i++;
        printf("%*s", HEX_LINE_LEN - 24 - (i << 1) - (i >> 1), ""); // add whitespace
    }
    

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
            outputLine(offset, bytes);
            i = 0;
            memset(&bytes[0], 0, sizeof(bytes));

            if (offset == UINT_MAX - 0xF) offset = 0x0;
            else offset += 0x10;
        }

        if (is_bin && i == MAX_BIN_BYTE_COUNT) {
            outputLine(offset, bytes);
            i = 0;
            memset(&bytes[0], 0, sizeof(bytes));

            if (offset == UINT_MAX - 0xF) offset = 0x0;
            else offset += 0x6;
        }
    }

    outputLine(offset, bytes);
}

int main(int argc, char *argv[]) {
    int opt;
    static struct option long_options[] = {
        {"len",         required_argument,  NULL,   'l'},
        {"bin",         no_argument,        NULL,   'b'},
        {"groupsize",   required_argument,  NULL,   'g'},
        {0, 0, 0, 0}
    };

    int opt_index = 0;
    while ((opt = getopt_long(argc, argv, "l:bg:", long_options, &opt_index)) != -1) {
        
        switch (opt) {
            errno = 0;
            char *endptr;

            case 'l':
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

            case 'g':
                bytes_per_group = strtol(optarg, &endptr, 0);
                
                if (bytes_per_group < CHAR_MIN || bytes_per_group > CHAR_MAX) {
                    errno = ERANGE;
                }

                if (errno == ERANGE) {
                    fprintf(stderr, "cxd: len value out of range\n");
                    return 1;
                }
                if (*endptr != '\0') {
                    fprintf(stderr, "cxd: invalid len value: %s\n", optarg);
                    return 1;
                }

                bytes_per_group = (char)bytes_per_group;

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