#include "window.h"

#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shader.h"
#include "global.h"
#include "text_draw.h"
#include <limits.h>

static Window *window;
static EditorVisuals *visuals;

void
InitWindow()
{
    window = (Window *)malloc(sizeof(Window));
    visuals = (EditorVisuals *)malloc(sizeof(EditorVisuals));
    
    visuals->textFont = MakeFont("LiberationMono-Regular", FONT_SIZE);
    window->openFiles = (Array *)malloc(sizeof(Array));
    ArrayInit(window->openFiles);

    window->panelOne = new Panel();
    window->panelTwo = new Panel();
    
    PanelInit(window->panelOne, visuals->textFont, NULL, 0, 0, 640, 720);
    PanelInit(window->panelTwo, visuals->textFont, NULL, 645, 0, 1280, 720);
    
    window->currPanel = window->panelTwo;
    WindowOpenFile("TODO.txt");
    window->currPanel->statusBar = (char *)malloc(50 * sizeof(char));
    strcpy_s(window->currPanel->statusBar, 1 + strlen("test"), "test");
    
    window->currPanel = window->panelOne;
    WindowOpenFile("scratch.txt");
    window->currPanel->statusBar = (char *)malloc(50 * sizeof(char));
    strcpy_s(window->currPanel->statusBar, 1 + strlen("scratch.txt"), "scratch.txt");
    
    window->cmdCursorIndex = 0;
    window->mode = BUFFER_MODE;
    window->cmd = SAVE_CMD;

    window->cmdBuffer = CreateBuffer(50);

    // Initialize visuals
    // TODO: Decide if cmdBuffer coordinates should be stored here
    visuals->cmdBufferRect.x = 0.0f;
    visuals->cmdBufferRect.y = 0.0f;
    visuals->cmdBufferRect.width = WIDTH;
    visuals->cmdBufferRect.height = 0.03f * HEIGHT;

    visuals->statusRectOne.x = 0.0f;
    visuals->statusRectOne.y = 0.03f * HEIGHT;
    visuals->statusRectOne.width = window->panelOne->width;
    visuals->statusRectOne.height = 0.03f * HEIGHT;

    visuals->statusRectTwo.x = window->panelTwo->startX;
    visuals->statusRectTwo.y = 0.03f * HEIGHT;
    visuals->statusRectTwo.width = window->panelTwo->width;
    visuals->statusRectTwo.height = 0.03f * HEIGHT;

    // Colors: TODO: Hotload this
    visuals->backgroundColor.x = 24.0f;
    visuals->backgroundColor.y = 20.0f;
    visuals->backgroundColor.z = 24.0f;
    
    visuals->statusBarColor.x =  191.0744f;
    visuals->statusBarColor.y = 194.304f;
    visuals->statusBarColor.z = 191.0744f;
    
    visuals->textColor.x = 210.0f;
    visuals->textColor.y = 213.0f;
    visuals->textColor.z = 220.0f;
    
    visuals->statusBarTextColor.x = 17.075f;
    visuals->statusBarTextColor.y = 16.0f;
    visuals->statusBarTextColor.z = 19.99f;
    
    visuals->cursorColor.x = 217.0f;
    visuals->cursorColor.y = 218.0f;
    visuals->cursorColor.z = 217.0f;
    
    glm::mat4 projection = glm::ortho((GLfloat)0, (GLfloat)(WIDTH), (GLfloat)0, (GLfloat)(HEIGHT));
    
    // Text
    visuals->textShader = (Shader *)malloc(sizeof(Shader));
    InitShader(visuals->textShader, "shaders/text.vs", "shaders/text.fs");
    glUseProgram(visuals->textShader->program);
    glUniformMatrix4fv(glGetUniformLocation(visuals->textShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
    // Quads
    visuals->rectShader = (Shader *)malloc(sizeof(Shader));
    InitShader(visuals->rectShader, "shaders/rect.vs", "shaders/rect.fs");
    glUseProgram(visuals->rectShader->program);
    glUniformMatrix4fv(glGetUniformLocation(visuals->rectShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));    

}

void
WindowUpdate()
{
    if (window->panelOne != NULL) {
        PanelUpdate(window->panelOne);
    }
    if (window->panelTwo != NULL) {
        PanelUpdate(window->panelTwo);
    }
}

void
WindowResize(int width, int height)
{
    window->panelOne->width = width / 2 - 5;
    window->panelOne->height = height;
    window->panelTwo->startX = width / 2 + 5;
    window->panelTwo->width = width;
    window->panelTwo->height = height;
    
    visuals->statusRectOne.width = window->panelOne->width;
    visuals->statusRectTwo.x = window->panelTwo->startX;

    window->panelOne->maxCharsInLine = (float)(window->panelOne->width - window->panelOne->startX) / (float)(visuals->textFont->characterWidth);
    window->panelTwo->maxCharsInLine = (float)(window->panelTwo->width - window->panelTwo->startX) / (float)(visuals->textFont->characterWidth);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
    glUseProgram(visuals->textShader->program);
    glUniformMatrix4fv(glGetUniformLocation(visuals->textShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(visuals->rectShader->program);
    glUniformMatrix4fv(glGetUniformLocation(visuals->rectShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void
WindowOpenFile(char *path)
{
    for (int i = 0; i < ArrayLength(window->openFiles); i++) {
        BufferFile *file = (BufferFile *)ArrayGet(window->openFiles, i);
        
        if (strcmp((file)->path, path) == 0) {
            window->currPanel->visitedFile = file;
            return;
        }
    }
    
    BufferFile *newFile = new BufferFile();
    char *fileResult = ReadEntireFile(path);
    
    if(fileResult == NULL) {
        free(fileResult);
        delete(newFile);
        return;
    }
    
    newFile->buffer = CreateBuffer(strlen(fileResult) * 2 + 2);
    newFile->path = (char *)malloc(strlen(path)+1);
    strcpy_s(newFile->path, 1+strlen(path), path);
    
    newFile->currActionPos = -1;
    
    BufferSetText(newFile->buffer, fileResult, 0);
    
    window->currPanel->visitedFile = newFile;
    window->currPanel->cursorIndex = 0;
    
    ArrayAdd(window->openFiles, (void *)newFile);
    BufferCalcLines(window->currPanel->visitedFile->buffer, window->currPanel->linesArr, window->currPanel->maxCharsInLine);
    window->currPanel->linesSynced = true;

    free(fileResult);
}


void
WindowFree()
{
    ArrayFree(window->openFiles);
    free(window->openFiles);
    PanelFree(window->panelOne);
    PanelFree(window->panelTwo);
    free(window->panelOne);
    free(window->panelTwo);
}

void 
WindowInsertCharacter(char c)
{
    switch(window->mode) {
        case BUFFER_MODE:
        {
            PanelInsertCharacter(window->currPanel, c, true);
        } break;
        
        case COMMAND_BUFFER_MODE:
        {
            BufferInsertCharacter(window->cmdBuffer, c);
            window->cmdCursorIndex++;
        } break;
    }
}

void 
WindowDeleteCharacter()
{
    switch(window->mode) {
    case BUFFER_MODE:
        {
            PanelDeleteCharacter(window->currPanel, true);
        } break;
        
    case COMMAND_BUFFER_MODE:
        {
            BufferDeleteCharacter(window->cmdBuffer);
            
            window->cmdCursorIndex--;
            if (window->cmdCursorIndex < 0) {
                window->cmdCursorIndex = 0;
            }
        } break;
    }
    
}

void
WindowUndo()
{
    if (window->mode) {
        PanelUndo(window->currPanel);
    }
}

void
WindowRedo()
{
    if (window->mode) {
        PanelRedo(window->currPanel);
    }
}

static void
DrawGapBufferAndCursor(GapBuffer *buffer,
                       Array *linesArr,
		       i8 lineLength, int cursorIndex, 
		       GLfloat x, GLfloat xInit, GLfloat y, GLfloat yTop, GLfloat yBottom)
{
    
    x = xInit;
    GLfloat cursorX;
    vec3 textColor = visuals->textColor;

    glUseProgram(visuals->textShader->program);

    // Find the line that the cursor index is at
    GLfloat advance = visuals->textFont->characters['w'].Advance >> 6;
    
    for (int i = 0; i < ArrayLength(linesArr); i++) {

        if (y - (GLint)(LINE_SPACING * FONT_SIZE) > yTop) {
            y -= (GLint)(LINE_SPACING * FONT_SIZE);
            continue;
        }
        
        int rowIndex = (int)ArrayGet(linesArr, i);
        int nextRowIndex = (i == ArrayLength(linesArr) - 1) ? INT_MAX : (int)ArrayGet(linesArr, i + 1);

        if (cursorIndex >= rowIndex && cursorIndex < nextRowIndex) {
            
            // Cursor row
            DrawGapBufferLine(visuals->textFont, buffer, rowIndex, 0, cursorIndex - rowIndex,
                              x, y, 0.6f,
                              textColor);

            x = xInit + (advance * (cursorIndex - rowIndex));

            glUseProgram(visuals->rectShader->program);
            GLDrawQuad(x, y - visuals->textFont->characterHeight / 6.0f, 0.0f,
                       visuals->textFont->characterWidth, visuals->textFont->characterHeight);

            textColor = visuals->backgroundColor;
            glUseProgram(visuals->textShader->program);

            char currChar;
            if (cursorIndex < buffer->preLength) {
                currChar = *(buffer->buffer + i);
            } else {
                currChar = *(buffer->gapEnd + i - buffer->preLength);
            }

            DrawGapBufferLine(visuals->textFont, buffer,
                              cursorIndex, cursorIndex - rowIndex, cursorIndex - rowIndex + 1,
                              x, y, 0.6f,
                              textColor);
                        
            // Draw rest of text
            textColor = visuals->textColor;
            DrawGapBufferLine(visuals->textFont, buffer, cursorIndex, cursorIndex - rowIndex, lineLength,
                              x, y, 0.6f,
                             textColor);
        } else {
            DrawGapBufferLine(visuals->textFont, buffer, rowIndex, 0, lineLength, x, y, 0.6f, textColor);
        }
        
        y -= (GLint)(LINE_SPACING * FONT_SIZE);

        if (y < yBottom) {
            break;
        }
        x = xInit;
    }
}

void
DrawEditor() 
{
    vec3 textColor = visuals->textColor;
    vec3 rectColor = visuals->statusBarColor;

    rectColor = visuals->cursorColor;
    // Draw panels
    {
        if (window->panelTwo != NULL) {
            Panel *panel = window->panelTwo;
            glUseProgram(visuals->rectShader->program);
            glUniform3f(glGetUniformLocation(visuals->rectShader->program, "rectColor"),
                        rectColor.x, rectColor.y, rectColor.z);

	
            glUseProgram(visuals->textShader->program);

            GLfloat x = panel->startX;
            GLfloat y = panel->height - FONT_SIZE + panel->yOffset - panel->startY;
            GLfloat xBound = panel->width;
            GLfloat xInit = panel->startX;
                
            DrawGapBufferAndCursor(panel->visitedFile->buffer,
                                   panel->linesArr,
                                   panel->maxCharsInLine,
                                   panel->cursorIndex,
                                   x, xInit,
                                   y, panel->height, 0);

        }
        if (window->panelOne != NULL) {
            glUseProgram(visuals->rectShader->program);
            glUniform3f(glGetUniformLocation(visuals->rectShader->program, "rectColor"),
                        rectColor.x, rectColor.y, rectColor.z);
            glUseProgram(visuals->textShader->program);
            Panel *panel = window->panelOne;
            GLfloat x = panel->startX;
            GLfloat y = panel->height - FONT_SIZE + panel->yOffset - panel->startY;
            GLfloat xBound = panel->width;
            GLfloat xInit = panel->startX;
            
            if (panel->linesSynced == false) {
                BufferCalcLines(panel->visitedFile->buffer, panel->linesArr, panel->maxCharsInLine);
                panel->linesSynced = true;
            }
            DrawGapBufferAndCursor(panel->visitedFile->buffer,
                                   panel->linesArr,
                                   panel->maxCharsInLine,
                                   panel->cursorIndex,
                                   x, xInit,
                                   y, panel->height, 0);
        }
    }
    // Draw status bar
    textColor = visuals->statusBarTextColor;
    rectColor = visuals->statusBarColor;
    {
        glUseProgram(visuals->rectShader->program);
        glUniform3f(glGetUniformLocation(visuals->rectShader->program, "rectColor"),
                    rectColor.x, rectColor.y, rectColor.z);

        if (window->panelTwo != NULL) {
            Panel *panel = window->panelTwo;

            GLDrawQuad(visuals->statusRectOne.x,
                       visuals->statusRectOne.y,
                       0.9f, 
                       visuals->statusRectOne.width,
                       visuals->statusRectOne.height);
        }
        if (window->panelOne != NULL) {
            Panel *panel = window->panelOne;

            GLDrawQuad(visuals->statusRectTwo.x,
                       visuals->statusRectTwo.y,
                       0.9f, 
                       visuals->statusRectTwo.width,
                       visuals->statusRectTwo.height); 
        }
        
        glUseProgram(visuals->textShader->program);

        

        int colNum = 0, rowNum = 0;

        // sprintf_s(panel->statusBar, 50, "%s\t%d, %d", panel->visitedFile->path, colNum, rowNum);
        if (window->panelOne != NULL) {
            Panel *panel = window->panelOne;
            GLfloat x = panel->startX;
            GLfloat y = (GLuint)(2 * FONT_SIZE / 1.1f);
            DrawText(visuals->textFont,
                     panel->statusBar, 
                     x, y, 1.0f,
                     textColor);

        }

        if (window->panelTwo != NULL) {
            Panel *panel = window->panelTwo;
        
            GLfloat x = panel->startX;
            GLfloat y = (GLuint)(2 * FONT_SIZE / 1.1f);

            DrawText(visuals->textFont,
                     panel->statusBar,
                     x, y, 1.0f, textColor);
        }
	
    }
    
    // Draw command buffer
    rectColor = visuals->backgroundColor;
    textColor = visuals->textColor;
    {
        GLuint rectShader = visuals->rectShader->program;
        glUseProgram(rectShader);
        glUniform3f(glGetUniformLocation(rectShader, "rectColor"), 
                    rectColor.x, rectColor.y, rectColor.z);
        GLDrawQuad(visuals->cmdBufferRect.x,
                   visuals->cmdBufferRect.y,
                   0.9f,
                   visuals->cmdBufferRect.width,
                   visuals->cmdBufferRect.height);

        if (window->mode == COMMAND_BUFFER_MODE) {
            GLfloat advance = visuals->textFont->characters['w'].Advance >> 6;
            GLint x = visuals->cmdBufferRect.x + (advance * window->cmdCursorIndex);
            GLint y = visuals->cmdBufferRect.y + visuals->cmdBufferRect.height / 4;

            glUseProgram(visuals->textShader->program);
            DrawGapBufferLine(visuals->textFont, window->cmdBuffer,
                              0, 0, window->cmdCursorIndex,
                              visuals->cmdBufferRect.x, y, 1.0f,
                              textColor);
            rectColor = visuals->cursorColor;
            glUseProgram(visuals->rectShader->program);
            glUniform3f(glGetUniformLocation(rectShader, "rectColor"), 
                        rectColor.x, rectColor.y, rectColor.z);
            GLDrawQuad(x, y, 0.95f,
                       visuals->textFont->characterWidth, visuals->textFont->characterHeight);

            glUseProgram(visuals->textShader->program);
            DrawGapBufferLine(visuals->textFont, window->cmdBuffer,
                              window->cmdCursorIndex, window->cmdCursorIndex,
                              100, x, y, 1.0f, textColor);
        }
    }
}

void
WindowMoveCursor(int colOffset, int rowOffset)
{
    switch(window->mode) {
        case BUFFER_MODE:
        {
            PanelMoveCursor(window->currPanel, colOffset, rowOffset);
        } break;
        
        case COMMAND_BUFFER_MODE:
        {
            window->cmdCursorIndex += colOffset;
            
            if (window->cmdCursorIndex < 0) {
                window->cmdCursorIndex = 0;
            }  else if (((int)window->cmdCursorIndex - (int)window->cmdBuffer->preLength >= window->cmdBuffer->postLength) && colOffset > 0){
                window->cmdCursorIndex = window->cmdBuffer->postLength + window->cmdBuffer->preLength;
            }
        } break;
    }
    
}

void
WindowMoveStartOfLine()
{
    PanelMoveStartOfLine(window->currPanel);
    
}

void 
WindowMoveEndOfLine()
{
    PanelMoveEndOfLine(window->currPanel);
}

void 
WindowMoveStartOfWord()
{
    PanelMoveStartOfWord(window->currPanel);
}

void 
WindowMoveToNextSpace()
{
    PanelMoveToNextSpace(window->currPanel);
}

void 
WindowMoveToEmptyLine(int rowChange)
{
    PanelMoveToEmptyLine(window->currPanel, rowChange);
}

void
WindowScroll(GLfloat amount)
{
    window->currPanel->yOffset = window->currPanel->yOffset + amount * 7.0f;
    if (window->currPanel->yOffset < 0.0f) {
        window->currPanel->yOffset = 0.0f;
    }
    
}

void
WindowNextPanel()
{
    if (window->currPanel == window->panelOne) {
        window->currPanel = window->panelTwo;
    } else {
        window->currPanel = window->panelOne;
    }
}

void
WindowSetMark()
{
    PanelSetMark(window->currPanel);
}

void
WindowCopy()
{
    PanelCopy(window->currPanel);
}

void
WindowPaste()
{
    PanelPaste(window->currPanel);
}

void
WindowSetCommand(int command)
{
    window->cmd = command;
    window->mode = COMMAND_BUFFER_MODE;
}

void
WindowEvaluate()
{
    switch(window->mode) {
        case BUFFER_MODE:
        {
            PanelInsertCharacter(window->currPanel, '\n', true);
        } break;
        
        case COMMAND_BUFFER_MODE:
        {
            switch(window->cmd) {
                case OPEN_CMD:
                {
                    char *buffString = BufferGetText(window->cmdBuffer);
                    
                    WindowOpenFile(buffString);
                    
                    free(buffString);
                    
                    BufferSetText(window->cmdBuffer, "", 0);
                    window->cmdCursorIndex = 0;
                } break;
                
                case SAVE_CMD:
                {
                    char *textString = BufferGetText(window->currPanel->visitedFile->buffer);
                    SaveStringToFile(window->currPanel->visitedFile->path, textString);
                    free(textString);
                    
                    // TODO:
                    BufferSetText(window->cmdBuffer, "", 0);
                    window->cmdCursorIndex = 0;
                } break;
                
                case FIND_CMD:
                {
                    char *buffString = BufferGetText(window->cmdBuffer);
                    
                    int index = WindowFindString(buffString);
                    
                    if (index + 1 >= 0) {
                        window->currPanel->cursorIndex = index;
                    }
                    
                    free(buffString);
                    
                    BufferSetText(window->cmdBuffer, "", 0);
                    window->cmdCursorIndex = 0;
                } break;
                
                case RFIND_CMD:
                {
                } break;
            }

            window->mode = BUFFER_MODE;
        } break;
    }
}

char *reverse(const char *s)
{
    if (s == NULL)
        return NULL;
    size_t i, len = strlen(s);
    char *r = (char *)malloc(len + 1);
    
    for(i = 0; i < len; ++i)
        r[i] = s[len - i - 1];
    r[len] = 0;
    return r;
}

int
WindowFindStringReverse(char *query)
{
    char *text = BufferGetText(window->currPanel->visitedFile->buffer);
    char *rtext = reverse(text);
    char *rquery = reverse(query);
    
    
    char *match = strstr(rtext + strlen(rtext) - window->currPanel->cursorIndex - 1, 
                         rquery);
    
    if (match == NULL) {
        free(text);
        free(rtext);
        free(rquery);
        return -2;
    } 
    
    int end = strlen(rtext) - (match - rtext) - strlen(rquery) - 1;
    free(text);
    free(rtext);
    free(rquery);
    
    return end;
}


int
WindowFindString(char *query)
{
    char *text = BufferGetText(window->currPanel->visitedFile->buffer);
    char *match = strstr(text + window->currPanel->cursorIndex + 1, 
                         query);
    if (match == NULL) {
        free(text);
        return -2;
    } 
    
    int end = match - text - 1;
    free(text);
    
    return end;
}

void
WindowChangeMode(int mode)
{
    window->mode = mode;
}


void
WindowClosePanel()
{
    if (window->panelOne != NULL && window->panelTwo != NULL) {
        Panel *other = NULL;
        if (window->currPanel == window->panelOne) {
            other = window->panelTwo;
            other->startX = window->panelOne->startX;
            other->width = WIDTH;
            other->height = HEIGHT;
            visuals->statusRectTwo.x = other->startX;
            visuals->statusRectTwo.width = WIDTH;
            other->maxCharsInLine = (float)(other->width - other->startX) / (float)(visuals->textFont->characterWidth);
            window->panelOne = NULL;
        } else {
            other = window->panelOne;
            other->maxCharsInLine = (float)(other->width - other->startX) / (float)(visuals->textFont->characterWidth);
            visuals->statusRectOne.width = WIDTH;
            other->width = WIDTH;
            other->height = HEIGHT;
            window->panelTwo = NULL;
        }
        PanelFree(window->currPanel);
        free(window->currPanel);

        window->currPanel = other;
        BufferCalcLines(other->visitedFile->buffer, other->linesArr, other->maxCharsInLine);

    }
}
