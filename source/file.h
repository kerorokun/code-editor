#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include "buffer.h"
#include "action.h"

#include <stack>
#include "array.h"
#include "editor_platform.h"

#define MAX_ACTIONS 500

typedef struct {
    GapBuffer *buffer;
    char *path;
    int numLines;
    
    // Action actions[MAX_ACTIONS]; 
    
    std::stack<Action> undoActions;
    std::stack<Action> redoActions;
    
    int currActionPos;

    Array *linesArr;
} BufferFile;

void SaveStringToFile(char *path, char *string);
char * ReadEntireFile(const char *path);

#endif
