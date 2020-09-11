
#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
SaveStringToFile(char *path, char *string)
{
    FILE *fp;
    
    fopen_s(&fp, path, "w");
    fprintf(fp, "%s", string);
    
    fclose(fp);
}


char *
ReadEntireFile(const char *path)
{
    FILE *fp;
    long fileLength;
    char *buffer;
    
    // TODO: Handle the errno returned from fopen_s
    // TODO: Add #ifdef for windows vs linux?
    fopen_s(&fp, path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "(ReadEntireFile) Failed to open file: %s", path);
        return NULL;
    }
    
    fseek(fp, 0L, SEEK_END);
    fileLength = ftell(fp);
    rewind(fp);
    
    buffer = (char *)malloc(fileLength + 1);
    if (buffer == NULL) {
        perror("(ReadEntireFile) Error mallocing: ");
        return NULL;
    }
    
    memset(buffer, 0, fileLength + 1);
    
    int c;
    int index = 0;
    size_t n = 0;
    
    fread(buffer, fileLength, 1, fp);
    buffer[fileLength] = '\0';
    fclose(fp);
    
    return buffer;
}
