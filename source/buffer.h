#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <vector>
#include "array.h"

typedef struct {
    char *buffer;
    
    char *gapStart;
    char *gapEnd;
    
    int preLength;
    int postLength;
    int maxSize;
} GapBuffer;

GapBuffer * CreateBuffer(const size_t size);
void InitBuffer(GapBuffer *buffer, size_t size);
void FreeBuffer(GapBuffer *buffer);

void BufferMoveBack(GapBuffer *buffer, const unsigned int amount);
void BufferMoveForward(GapBuffer *buffer, const unsigned int amount);
void BufferInsertCharacter(GapBuffer *buffer, char c);
char BufferDeleteCharacter(GapBuffer *buffer);

int BufferGetCursorIndex(GapBuffer *buffer);
char * BufferGetText(GapBuffer *buffer);
void BufferSetText(GapBuffer *buffer, char *string, const unsigned int offset);

void BufferExpandGap(GapBuffer *buffer, const unsigned int amount);
void BufferCalcLines(GapBuffer *buffer, Array *lines, int maxCharsInLine);
char BufferGetCharAt(GapBuffer *buffer, int cursorPos);

char *BufferFindString(GapBuffer *buffer, const char *substr);

#endif 
