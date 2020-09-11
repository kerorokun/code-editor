#include "panel.h"
#include "buffer.h"
#include "shader.h"
#include "global.h"
#include "text.h"

#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <string>

void 
PanelInit(Panel *panel, Font *font, BufferFile *file, GLuint startX, GLuint startY, GLuint width, GLuint height)
{
    panel->visitedFile = file;
    panel->width = width;
    panel->height = height;
    
    panel->cursorIndex = 0;
    panel->markIndex = -1;
    panel->yOffset = 0;
    panel->linesSynced = false;
    
    panel->savedRowIndex = -1;
    panel->startRow = 0;
    
    panel->startX = startX;
    panel->startY = startY;
    
    panel->linesArr = (Array *)malloc(sizeof(Array));
    ArrayInit(panel->linesArr);
    
    float maxChars = (float)(width - startX) / (float)(font->characterWidth);
    panel->maxCharsInLine = maxChars;
}

void
PanelUpdate(Panel *panel) 
{
    if (panel->linesSynced == false) {
        BufferCalcLines(panel->visitedFile->buffer, panel->linesArr, panel->maxCharsInLine);
        panel->linesSynced = true;
    }
}


void 
PanelAdjustViewspace(Panel *panel) 
{
    int offset = 0;
    int yThres = 0;
    
    int numberOfRows = (panel->height - FONT_SIZE) / (FONT_SIZE * LINE_SPACING);
    int startRow = (panel->yOffset - FONT_SIZE / 2) / (FONT_SIZE * LINE_SPACING);
    if (startRow < 0) { 
        startRow = 0;
    }
    
    int cursorRow = 0;
    
    if (panel->linesSynced == false) {
        BufferCalcLines(panel->visitedFile->buffer, panel->linesArr, panel->maxCharsInLine);
        panel->linesSynced = true;
    }
    for (int i = ArrayLength(panel->linesArr) - 1; i >= 0; i--) {
        int rowIndex = (int)ArrayGet(panel->linesArr, i);
        if (panel->cursorIndex  >= rowIndex) {
            cursorRow = i;
            break;
        }
    }
    
    // TODO: Make estimate of calculation
    // NOTE: 15 = size of { character with FONT_SIZE 16
    // Going up: If the bottom of the cursor is above the top of the line
    if ((int)(panel->height - FONT_SIZE + panel->yOffset - panel->startY)
        - (cursorRow + yThres) * (GLint)(FONT_SIZE * LINE_SPACING) + 15 >
        (int)(panel->height - FONT_SIZE + panel->yOffset - panel->startY)
        - (startRow) * (GLint)(FONT_SIZE * LINE_SPACING)) {
        panel->yOffset -= panel->height / 2;
        if (panel->yOffset < 0) {
            panel->yOffset = 0;
        }
    }
    
    // Going down: If the top of the cursor is below the bottom of the line
    if ((int)(panel->height - FONT_SIZE * 6 + panel->yOffset)
        - (cursorRow + yThres) * (GLint)(FONT_SIZE * LINE_SPACING) < 0) {
        panel->yOffset += panel->height / 2;
    }
    
}

void 
PanelMoveCursor(Panel *panel, int colChange, int rowChange)
{
    // Handle horizontally
    {
        if (colChange != 0) {
            panel->savedRowIndex = -1;
        }
        panel->cursorIndex += colChange;
        
        if (panel->cursorIndex < 0) {
            panel->cursorIndex = 0;
        }
        
        if (panel->cursorIndex - panel->visitedFile->buffer->preLength >= panel->visitedFile->buffer->postLength){
            panel->cursorIndex = panel->visitedFile->buffer->postLength + panel->visitedFile->buffer->preLength;
        }        
    }
    
    // Handle vertically
    if (rowChange != 0) {
        // TODO: Implement
        int row_to_do;
        
        if (panel->savedRowIndex < 0) {
            int saveCol = 0;
            
            if (panel->linesSynced == false) {
                BufferCalcLines(panel->visitedFile->buffer, panel->linesArr, panel->maxCharsInLine);
                panel->linesSynced = true;
            }
            
            // TODO: Move this loop to automatically populate int row above;
            for (int i = ArrayLength(panel->linesArr) - 1; i >= 0; i--) {
                int rowIndex = (int)ArrayGet(panel->linesArr, i);
                if (panel->cursorIndex >= rowIndex) {
                    saveCol = panel->cursorIndex - rowIndex;
                    break;
                }
            }
            
            panel->savedRowIndex = saveCol;
        }
        
        if (rowChange < 0) {
            
            int cursorRow = 0;
            for (int i = ArrayLength(panel->linesArr) - 1; i >= 0; i--) {
                int rowIndex = (int)ArrayGet(panel->linesArr, i);
                if (panel->cursorIndex  >= rowIndex) {
                    cursorRow = i;
                    break;
                }
            }
            
            if (cursorRow == 0) {
                panel->cursorIndex = 0;
                return;
            }
            panel->cursorIndex = (int)ArrayGet(panel->linesArr, cursorRow - 1);
            
            for (int i = 0; i < panel->savedRowIndex; i++, panel->cursorIndex++) {
                char currChar;
                if (panel->cursorIndex < panel->visitedFile->buffer->preLength) {
                    currChar = *(panel->visitedFile->buffer->buffer + panel->cursorIndex);
                } else {
                    currChar = *(panel->visitedFile->buffer->gapEnd + panel->cursorIndex - panel->visitedFile->buffer->preLength);
                }
                if (currChar == '\n' || currChar < 0) {
                    break;
                }
            }
            
        } else if (rowChange > 0) {
            int cursorRow = 0;
            for (int i = ArrayLength(panel->linesArr) - 1; i >= 0; i--) {
                int rowIndex = (int)ArrayGet(panel->linesArr, i);
                if (panel->cursorIndex >= rowIndex) {
                    cursorRow = i;
                    break;
                }
            }
            
            if (cursorRow == ArrayLength(panel->linesArr) - 1) {
                panel->cursorIndex = panel->visitedFile->buffer->postLength + panel->visitedFile->buffer->preLength;
                return;
            }
            
            panel->cursorIndex = (int)ArrayGet(panel->linesArr, cursorRow + 1);
            
            for (int i = 0; i < panel->savedRowIndex; i++, panel->cursorIndex++) {
                char currChar;
                if (panel->cursorIndex < panel->visitedFile->buffer->preLength) {
                    currChar = *(panel->visitedFile->buffer->buffer + panel->cursorIndex);
                } else {
                    currChar = *(panel->visitedFile->buffer->gapEnd + panel->cursorIndex - panel->visitedFile->buffer->preLength + 1);
                }
                if (currChar == '\n' || currChar < 0) {
                    break;
                }
            }
        }        
    }
    
    PanelAdjustViewspace(panel);
}

void 
PanelMoveToEmptyLine(Panel *panel, int rowChange)
{
    bool foundLine = false;
    
    if (rowChange < 0) {
        // Scroll up
        if (!panel->linesSynced) {
            BufferCalcLines(panel->visitedFile->buffer, panel->linesArr, panel->maxCharsInLine);
            panel->linesSynced = true;
        }
        int row = 0;
        for (int i = ArrayLength(panel->linesArr) - 1; i >= 0; i--) {
            int rowIndex = (int)ArrayGet(panel->linesArr, i);
            if (panel->cursorIndex >= rowIndex) {
                row = i;
                break;
            }
        }
        while (!foundLine) {
            if (row == 0) {
                panel->cursorIndex = 0;
                foundLine = true;
            } else {
                panel->cursorIndex = (int)ArrayGet(panel->linesArr, row - 1);
                char currChar;
                if (panel->cursorIndex < panel->visitedFile->buffer->preLength) {
                    currChar = *(panel->visitedFile->buffer->buffer + panel->cursorIndex);
                } else {
                    currChar = *(panel->visitedFile->buffer->gapEnd + panel->cursorIndex - panel->visitedFile->buffer->preLength + 1);
                }
                if (currChar == '\n') {
                    foundLine = true;
                }
                row--;
            }
        }
        PanelAdjustViewspace(panel);
    } else if (rowChange > 0) {
        // Scroll down
        if (!panel->linesSynced) {
            BufferCalcLines(panel->visitedFile->buffer, panel->linesArr, panel->maxCharsInLine);
            panel->linesSynced = true;
        }
        int row = 0;
        for (int i = ArrayLength(panel->linesArr) - 1; i >= 0; i--) {
            int rowIndex = (int)ArrayGet(panel->linesArr, i);
            if (panel->cursorIndex >= rowIndex) {
                row = i;
                break;
            }
        }
        while (!foundLine) {
            if (row == ArrayLength(panel->linesArr) - 1) {
                panel->cursorIndex = panel->visitedFile->buffer->preLength + panel->visitedFile->buffer->postLength;
                foundLine = true;
            } else {
                panel->cursorIndex = (int)ArrayGet(panel->linesArr, row + 1);
                char currChar;
                if (panel->cursorIndex < panel->visitedFile->buffer->preLength) {
                    currChar = *(panel->visitedFile->buffer->buffer + panel->cursorIndex);
                } else {
                    currChar = *(panel->visitedFile->buffer->gapEnd + panel->cursorIndex - panel->visitedFile->buffer->preLength + 1);
                }
                if (currChar == '\n') {
                    foundLine = true;
                }
                row++;
            }
        }
        PanelAdjustViewspace(panel);
    }
    
}

void
PanelMoveStartOfLine(Panel *panel)
{
    if (!panel->linesSynced) {
        BufferCalcLines(panel->visitedFile->buffer, panel->linesArr, panel->maxCharsInLine);
        panel->linesSynced = true;
    }
    int rowIndex;
    for (int i = ArrayLength(panel->linesArr) - 1; i >= 0; i--) {
        rowIndex = (int)ArrayGet(panel->linesArr, i);
        if (panel->cursorIndex >= rowIndex) {
            break;
        }
    }
    
    panel->cursorIndex = rowIndex;
    
    PanelAdjustViewspace(panel);
    panel->savedRowIndex = -1;
}

void
PanelMoveEndOfLine(Panel *panel)
{
    if (!panel->linesSynced) {
        BufferCalcLines(panel->visitedFile->buffer, panel->linesArr, panel->maxCharsInLine);
        panel->linesSynced = true;
    }
    int row = 0;
    for (int i = ArrayLength(panel->linesArr) - 1; i >= 0; i--) {
        int rowIndex = (int)ArrayGet(panel->linesArr, i);
        if (panel->cursorIndex >= rowIndex) {
            row = i;
            break;
        }
    }
    
    // Special situation: Last row
    if (row == ArrayLength(panel->linesArr) - 1) {
        panel->cursorIndex = panel->visitedFile->buffer->preLength + panel->visitedFile->buffer->postLength;
    } else {
        panel->cursorIndex = (int)ArrayGet(panel->linesArr, row + 1) - 1;
    }
    
    PanelAdjustViewspace(panel);
    panel->savedRowIndex = -1;
}

void
PanelMoveStartOfWord(Panel *panel)
{
    bool foundStart = false;
    int startPos = panel->cursorIndex;
    panel->savedRowIndex = -1;
    panel->cursorIndex--;
    
    
    if (panel->cursorIndex < 0) {
        panel->cursorIndex = 0;
        foundStart = true;
        
    }
    
    while (!foundStart) {
        panel->cursorIndex--;
        if (panel->cursorIndex < 0) {
            panel->cursorIndex = 0;
            foundStart = true;
            break;
        }
        
        char currChar;
        
        if (panel->cursorIndex < panel->visitedFile->buffer->preLength) {
            currChar = *(panel->visitedFile->buffer->buffer + panel->cursorIndex);
        } else {
            currChar = *(panel->visitedFile->buffer->gapEnd + panel->cursorIndex - panel->visitedFile->buffer->preLength + 1);
        }
        if (currChar == ' ' && startPos != panel->cursorIndex) {
            panel->cursorIndex++;
            foundStart = true;
            break;
        }
    }
    
    PanelAdjustViewspace(panel);
}

void
PanelMoveToNextSpace(Panel *panel)
{
    bool foundSpace = false;
    int startPos = panel->cursorIndex;
    panel->savedRowIndex = -1;
    
    
    while (!foundSpace) {
        panel->cursorIndex++;
        
        if (panel->cursorIndex - panel->visitedFile->buffer->preLength >= panel->visitedFile->buffer->postLength){
            panel->cursorIndex = panel->visitedFile->buffer->postLength + panel->visitedFile->buffer->preLength;
            foundSpace = true;
            break;
        }
        
        char currChar;
        if (panel->cursorIndex > panel->visitedFile->buffer->preLength) {
            currChar = *(panel->visitedFile->buffer->gapEnd + panel->cursorIndex - panel->visitedFile->buffer->preLength);
        } else {
            currChar = *(panel->visitedFile->buffer->buffer + panel->cursorIndex);
        }
        
        if (currChar == ' ' && startPos != panel->cursorIndex) {
            foundSpace = true;
            break;
        }        
    }
    PanelAdjustViewspace(panel);
    
}

void 
PanelInsertCharacter(Panel *panel, char c, bool editHistory)
{
    int offset = 0;
    
    offset = panel->cursorIndex - BufferGetCursorIndex(panel->visitedFile->buffer);
    
    if (offset < 0) {
        BufferMoveBack(panel->visitedFile->buffer, -offset);
    } else if (offset > 0) {
        BufferMoveForward(panel->visitedFile->buffer, offset);
    }
    
    BufferInsertCharacter(panel->visitedFile->buffer, c);
    panel->cursorIndex++;
    
    PanelAdjustViewspace(panel);
    
    if (editHistory) {
        Action a;
        a.action = INSERT_ACTION;
        a.editString = c;
        a.cursorIndex = panel->cursorIndex;
        panel->visitedFile->undoActions.push(a);
        
        // Clear out the stack
        while (!panel->visitedFile->redoActions.empty()){
            panel->visitedFile->redoActions.pop();
        }
    }
    
    panel->savedRowIndex = -1;
    panel->linesSynced = false;
}

void 
PanelDeleteCharacter(Panel *panel, bool editHistory)
{
    int offset = 0;
    
    offset = panel->cursorIndex - BufferGetCursorIndex(panel->visitedFile->buffer);
    
    if (offset < 0) {
        BufferMoveBack(panel->visitedFile->buffer, -offset);
    } else if (offset > 0) {
        BufferMoveForward(panel->visitedFile->buffer, offset);
    }
    
    char c = BufferDeleteCharacter(panel->visitedFile->buffer);
    
    panel->cursorIndex--;
    
    if (panel->markIndex != -1 && panel->cursorIndex < panel->markIndex){
        panel->markIndex--;
        
        if (panel->markIndex < 0) {
            panel->markIndex = 0;
        }
    }
    
    if (panel->cursorIndex < 0) {
        panel->cursorIndex = 0;
    }
    
    
    PanelAdjustViewspace(panel);
    
    if (editHistory) {
        Action a;
        a.action = DELETE_ACTION;
        a.cursorIndex = panel->cursorIndex;
        a.editString = c;
        panel->visitedFile->undoActions.push(a);
        
        // Clear out the stack
        while (!panel->visitedFile->redoActions.empty()){
            panel->visitedFile->redoActions.pop();
        }
    }
    
    panel->savedRowIndex = -1;
    panel->linesSynced = false;
}

void
PanelUndo(Panel *panel)
{
    if (panel->visitedFile->undoActions.empty()) {
        return;
    }
    
    // Pop off
    Action a = panel->visitedFile->undoActions.top();
    panel->visitedFile->undoActions.pop();
    
    for (int i = 0; i < a.editString.length(); i++) {
        // Execute
        if (a.action == INSERT_ACTION) {
            panel->cursorIndex = a.cursorIndex;
            PanelDeleteCharacter(panel, false);
        } else if (a.action == DELETE_ACTION) {
            panel->cursorIndex = a.cursorIndex;
            PanelInsertCharacter(panel, (char)a.editString[i], false);
        }
    }
    
    // Push onto redo
    panel->visitedFile->redoActions.push(a);
    
    panel->savedRowIndex = -1;
    panel->linesSynced = false;
    PanelAdjustViewspace(panel);
}

void 
PanelRedo(Panel *panel)
{
    if (panel->visitedFile->redoActions.empty()) {
        return;
    }
    
    // Pop off
    Action a = panel->visitedFile->redoActions.top();
    if (a.action == DELETE_ACTION) {
        panel->cursorIndex = a.cursorIndex + a.editString.length();
    } else if (a.action == INSERT_ACTION) {
        panel->cursorIndex = a.cursorIndex - 1;
    }
    panel->visitedFile->redoActions.pop();
    
    for (int i = 0; i < a.editString.length(); i++) {
        // Execute
        if (a.action == INSERT_ACTION) {
            PanelInsertCharacter(panel, (char)a.editString[i], false);
        } else if (a.action == DELETE_ACTION) {
            PanelDeleteCharacter(panel, false);
        }
    }
    // Push onto redo
    panel->visitedFile->undoActions.push(a);
    
    panel->savedRowIndex = -1;
    panel->linesSynced = false;
    PanelAdjustViewspace(panel);
    
}

void
PanelSetMark(Panel *panel)
{
    panel->markIndex = panel->cursorIndex;
}

void
PanelCopy(Panel *panel)
{
    if (panel->markIndex == panel->cursorIndex) {
        return;
    }
    
    int length = (panel->markIndex < panel->cursorIndex) ?
        panel->cursorIndex - panel->markIndex :
        panel->markIndex - panel->cursorIndex;
    
    length++;
    char *text = (char *)malloc(length * sizeof(char));

    if (panel->markIndex > panel->cursorIndex) {
        for (int i = 0; i < length; i++) {
            char currChar;
        
            if (panel->markIndex > panel->visitedFile->buffer->preLength) {
                currChar = *(panel->visitedFile->buffer->gapEnd + panel->markIndex - panel->visitedFile->buffer->preLength);
            } else {
                currChar = *(panel->visitedFile->buffer->buffer + panel->markIndex);
            }
        
            *(text + length + i - 1) = currChar;
            panel->markIndex--;
        }
    } else {
        for (int i = 0; i < length; i++) {
            char currChar;
        
            if (panel->markIndex > panel->visitedFile->buffer->preLength) {
                currChar = *(panel->visitedFile->buffer->gapEnd + panel->markIndex - panel->visitedFile->buffer->preLength);
            } else {
                currChar = *(panel->visitedFile->buffer->buffer + panel->markIndex);
            }
        
            *(text + i) = currChar;
            panel->markIndex++;
        }
    }
    
#if _WIN32
    OpenClipboard(NULL);
    HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE, length);
    memcpy(GlobalLock(hText), text, length);
    GlobalUnlock(hText);
    
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hText);
    CloseClipboard();
#endif
    
}

void
PanelPaste(Panel *panel)
{
#if _WIN32
    OpenClipboard(NULL);
    
    HANDLE hData = GetClipboardData(CF_TEXT);
    char *text = (char *)(GlobalLock(hData));
    
    Action a;
    a.action = INSERT_ACTION;
    a.editString = text;
    a.cursorIndex = panel->cursorIndex + 1;
    
    panel->visitedFile->undoActions.push(a);
    
    for (int i = 0; i < strlen(text); i++) {
        PanelInsertCharacter(panel, *(text + i), false);
    }
    
    GlobalUnlock(hData);
    CloseClipboard();
#endif
    
    panel->linesSynced = false;
}


void
PanelFree(Panel *panel)
{
    ArrayFree(panel->linesArr);
    free(panel->linesArr);
}

