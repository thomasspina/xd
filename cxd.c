#include <stdio.h>
#include <string.h>
#include <limits.h>

#define MAX_BYTE_COUNT 16

void printLine(u_int32_t offset, u_int8_t* bytes) {
    printf("%08x: ", offset);
    short i = 0;

    while (bytes[i] != '\0') {
        printf("%02x", bytes[i++]);
        if (i % 2 == 0) printf(" ");
    }
    printf(" ");

    i = 0;
    while (bytes[i] != '\0') {
        if (bytes[i] == '\n' || bytes[i] == '\t') printf(".");
        else printf("%c", bytes[i]);

        i++;
    }

    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) { 
        perror(argv[1]);
        return 1;
    }

    u_int8_t buff; // buffer for a byte
    short i = 0; // where we are in our hex line
    u_int8_t bytes[MAX_BYTE_COUNT + 1] = {0}; // our current hex line
    u_int32_t offset = 0x0;

    while (fread(&buff, sizeof(u_int8_t), 1, fp) > 0) {
        bytes[i++] = buff;

        if (i == MAX_BYTE_COUNT) {
            printLine(offset, bytes);
            i = 0;
            memset(&bytes[0], 0, sizeof(bytes));

            if (offset == UINT_MAX - 0xF) offset = 0x0;
            else offset += 0x10;
        }
    }

    printLine(offset, bytes);

    fclose(fp);
    return 0;
}