#include "buffer.h"
#include "util.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif


GapBuffer *
CreateBuffer(const size_t size)
{
    GapBuffer *buff = (GapBuffer *)malloc(sizeof(GapBuffer));
    buff->buffer = NULL;
    
    InitBuffer(buff, size);
    
    return buff;
}

void
InitBuffer(GapBuffer *buffer, size_t size)
{
    if (buffer->buffer != NULL) {
        free(buffer->buffer);
    }
    
    buffer->buffer = (char *)malloc(size * sizeof(char));
    buffer->gapStart = buffer->buffer;
    buffer->gapEnd = buffer->buffer + size;
    buffer->preLength = 0;
    buffer->postLength = 0;
    buffer->maxSize = size;
}

void
FreeBuffer(GapBuffer *buffer)
{
    free(buffer->buffer);
    free(buffer);
}

void
BufferMoveBack(GapBuffer *buffer, const unsigned int amount)
{
    int i;
    for (i = 0; i < amount; i++) {
        if (buffer->preLength == 0) {
            break;
        } 
        buffer->preLength--;
        buffer->postLength++;
        
        buffer->gapEnd--;
        *(buffer->gapEnd) = *(buffer->gapStart - 1);
        (buffer->gapStart)--;
    }
}

void
BufferMoveForward(GapBuffer *buffer, const unsigned int amount)
{
    int i;
    for (i = 0; i < amount; i++) {
        if (buffer->postLength == 0) {
            break;
        }
        
        buffer->preLength++;
        buffer->postLength--;
        
        *(buffer->gapStart) = *(buffer->gapEnd);
        buffer->gapStart++;
        (buffer->gapEnd)++;
    }
}

void
BufferInsertCharacter(GapBuffer *buffer, char c)
{
    if (buffer->gapStart == buffer->gapEnd - 1) {
        BufferExpandGap(buffer, 2 * (buffer->preLength + buffer->postLength));
    }
    *(buffer->gapStart) = c;
    buffer->gapStart++;
    buffer->preLength++;
}


char
BufferDeleteCharacter(GapBuffer *buffer)
{
    if (buffer->preLength == 0) {
        return 0;
    }
    char c = *(buffer->gapStart - 1);
    
    buffer->gapStart--;
    buffer->preLength--;
    
    return c;
}


// NOTE: Under construction
char *
BufferGetText(GapBuffer *buffer)
{
    char *string;
    int i;
    
    string = (char *)malloc(buffer->preLength + buffer->postLength + 2);
    
    for (i = 0; i < buffer->preLength; i++) {
        *(string + i) = *(buffer->buffer + i);
    }
    
    for (i = 0; i < buffer->postLength; i++) {
        *(string + buffer->preLength + i) = *(buffer->gapEnd + i);
    }
    
    /*
    strncpy_s(string, buffer->preLength, buffer->buffer, buffer->preLength);
    strncpy_s(string + buffer->preLength, buffer->postLength, buffer->gapEnd, buffer->postLength);
    */
    
    *(string + buffer->preLength + buffer->postLength) = '\0';
    
    return string;
}


void
BufferSetText(GapBuffer *buffer, char *string, const unsigned int offset)
{
    memset(buffer->buffer, 0, strlen(string));
    
    // TODO: Check for when string > size of buffer
    // TODO: Make ifdef WIN32
    // TODO: Factor in the offset
    // TODO: Check for off by one error
    for (int i = 0; i < strlen(string); i++) {
        *(buffer->buffer + i) = *(string + i);
    }
    
    buffer->gapStart = buffer->buffer + strlen(string);
    buffer->preLength = strlen(string);
    buffer->postLength = 0;
    buffer->gapEnd = buffer->buffer + buffer->maxSize;
}

int
BufferGetCursorIndex(GapBuffer *buffer) {
    return buffer->preLength;
}

void
BufferExpandGap(GapBuffer *buffer, const unsigned int amount)
{
    char *temp;
    char *posInString;
    int i = 0;
    
    // TODO: Check to make sure we should actually expand gap
    buffer->maxSize += amount;
    temp = (char *)malloc(buffer->maxSize * sizeof(char));
    
    i = 0;
    for (posInString = buffer->buffer; posInString != buffer->gapStart; posInString++) {
        *(temp + i) = *(posInString);
        i++;
    }
    buffer->gapStart = temp + i;
    
    posInString = buffer->gapEnd;
    for (i = 0; i < buffer->postLength; i++) {
        *(buffer->gapStart + amount + i) = *(posInString + i);
    }
    buffer->gapEnd = buffer->gapStart + amount;
    
    free(buffer->buffer);
    buffer->buffer = temp;
    
}

/*
 * Browses through the gapbuffer and finds the ends of the lines
 */
void
BufferCalcLines(GapBuffer *buffer, Array *lines, int maxCharsInLine)
{
    if (buffer == NULL || lines == NULL) {
        return;
    }
    ArrayFree(lines);
    ArrayInit(lines);
    
    ArrayAdd(lines, (void *)0);
    
    char currChar;
    int i = 0;
    int numChars = 0;
    

    for (int i = 0; i < buffer->preLength + buffer->postLength; i++, numChars++) {
        if (i < buffer->preLength) {
            currChar = *(buffer->buffer + i);
        } else {
            currChar = *(buffer->gapEnd + i - buffer->preLength);
        }
        if (currChar == '\n' || numChars == maxCharsInLine - 1) {
	    numChars = 0;
            ArrayAdd(lines, (void *)(i + 1));
        }
    }
}

// NOTE: Brute force way right now
char *
BufferFindString(GapBuffer *buffer, const char *substr)
{
    if (buffer == NULL) {
        return NULL;
    }
    
    if (substr == NULL) {
        return buffer->buffer;
    }
    
    char *pBuffer = (buffer->preLength == 0) ? buffer->gapEnd : buffer->buffer;
    
    char *pSubstr = (char *)substr;
    char *pAdv = buffer->buffer;
    
    while (*++pSubstr) {
        pAdv++;
    }
    
    while (*pAdv) {
        char *pBegin = pBuffer;
        pSubstr = (char *)substr;
        
        while (*pBuffer && *pSubstr && *pBuffer == *pSubstr) {
            pBuffer++;
            if (pBuffer == buffer->gapStart) {
                pBuffer = buffer->gapEnd;
            }
            pSubstr++;
        }
        
        if (*pSubstr == NULL) {
            return pBegin;
        }
        pBuffer = pBegin + 1;
        pAdv++;
    }
    
    return NULL;
    
}
