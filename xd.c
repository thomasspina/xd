#include <stdio.h>
#include <string.h>
#include <limits.h>

#define LINE_LENGTH 67
#define MAX_BYTE_COUNT 16

void printLine(u_int32_t offset, char* hex_line) {
    printf("%08x: %s\n", offset, hex_line);
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

    unsigned short buff; // buffer for two bytes
    short hex_count = 0; // where we are in our hex line
    char hex_line[MAX_BYTE_COUNT * 2 + 1]; // our current hex line
    int n_bytes_read; // number of bytes we read
    u_int32_t offset = 0x0;

    while ((n_bytes_read = fread(&buff, sizeof(char), 2, fp)) > 0) {
        
        // add two bytes that are in the buffer into our hex line buffer
        snprintf(hex_line + hex_count, 
                sizeof(hex_line) - hex_count, 
                "%.2x%.2x", 
                buff & 0xff, 
                (buff >> 8) & 0xff); 

        // increment where we are in the buffer
        hex_count += n_bytes_read * 2; // 2 hex char per byte

        
        if (hex_count == MAX_BYTE_COUNT * 2) {
            printLine(offset, hex_line);
            hex_count = 0;
            memset(&hex_line[0], 0, sizeof(hex_line));

            if (offset == UINT_MAX - 0xF) offset = 0x0;
            else offset += 0x10;
        }
    }

    printLine(offset, hex_line);
    
    

    fclose(fp);
    return 0;
}