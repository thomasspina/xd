#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_BYTE_COUNT 16
#define LINE_LEN 67

long byte_limit = -1; // global var for -l --len option flags

void printLine(unsigned int offset, char* bytes) {
    printf("%08x: ", offset);
    short i = 0;

    while (bytes[i] != '\0') {
        printf("%02x", bytes[i++]);
        if (i % 2 == 0) printf(" ");
    }
    i++;
    printf("%*s", LINE_LEN - 24 - (i << 1) - (i >> 1), ""); // add whitespace

    i = 0;
    while (bytes[i] != '\0') {
        if (bytes[i] == '\n' || bytes[i] == '\t') printf(".");
        else printf("%c", bytes[i]);

        i++;
    }

    printf("\n");
}

void readAndWrite(FILE *fp) {
    char buff; // buffer for a byte
    short i = 0; // where we are in our hex line
    char bytes[MAX_BYTE_COUNT + 1] = {0}; // our current hex line
    unsigned int offset = 0x0;

    long total_byte_count = 0;

    while (fread(&buff, sizeof(char), 1, fp) > 0) {
        bytes[i++] = buff;
        total_byte_count++;

        if (byte_limit > 0 && total_byte_count == byte_limit) break; // -l flag limit break
        
        if (i == MAX_BYTE_COUNT) {
            printLine(offset, bytes);
            i = 0;
            memset(&bytes[0], 0, sizeof(bytes));

            if (offset == UINT_MAX - 0xF) offset = 0x0;
            else offset += 0x10;
        }
    }

    printLine(offset, bytes);
}

int main(int argc, char *argv[]) {
    int opt;
    static struct option long_options[] = {
        {"len", required_argument, NULL, 'l'},
        {0, 0, 0, 0}
    };

    int opt_index = 0;
    while ((opt = getopt_long(argc, argv, "l:", long_options, &opt_index)) != -1) {
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

            default:

                break;
        }
    }

    FILE *fp = fopen(argv[argc - 1], "r");
    if (fp == NULL) { 
        perror(argv[argc - 1]);
        return 1;
    }

    readAndWrite(fp);

    fclose(fp);
    return 0;
}