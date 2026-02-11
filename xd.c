#include <stdio.h>

#define LINE_LENGTH 67
#define MAX_BYTE_COUNT 16

void printLine(char* hex_line) {
    printf("%s\n", hex_line);
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

    unsigned short buff;
    short hex_count = 0;
    char hex_line[MAX_BYTE_COUNT * 2 + 1]; // 
    int n_bytes_read;
    while ((n_bytes_read = fread(&buff, sizeof(char), 2, fp)) > 0) {
        snprintf(hex_line + hex_count, 
                sizeof(hex_line) - hex_count, 
                "%.2x%.2x", 
                buff & 0xff, 
                (buff >> 8) & 0xff); 
        hex_count += n_bytes_read * 2; // 2 hex char per byte



        if (hex_count == MAX_BYTE_COUNT * 2) {
            // printline
            printLine(hex_line);
            break;
        }
    }
    
    

    fclose(fp);
    return 0;
}