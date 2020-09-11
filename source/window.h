#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "file.h"
#include "panel.h"
#include "action.h"
#include "buffer.h"
#include "gl.h"
#include "shader.h"
#include "array.h"
#include "editor_platform.h"

#define BUFFER_MODE 1
#define COMMAND_BUFFER_MODE 2

#define WAITING_FOR_ARG 1

#define SAVE_CMD 1
#define OPEN_CMD 2
#define FIND_CMD 3
#define RFIND_CMD 4

enum WindowState {
    BUFFER_STATE,
    COMMAND_BUFFER_STATE
};

enum Command {
    SAVE,
    OPEN,
    FIND,
    RFIND
};

typedef struct {
    Array *openFiles;
    
    // TODO: Make into an array
    Panel *panelOne;
    Panel *panelTwo;

    Panel *panel[2];
    
    // Commandbuffer
    int cmdCursorIndex;
    GapBuffer *cmdBuffer;
    
    int mode;
    int cmd;
    
    std::map<GLchar, Character> characters;
    Panel *currPanel;
} Window;

typedef struct {
    vec3 backgroundColor;
    vec3 textColor;
    vec3 cursorColor;
    vec3 statusBarColor;
    vec3 statusBarTextColor;
    //vec3 highlightColor;
    //vec3 commentColor;
    //vec3 quoteColor;
    //vec3 constantColor;
    //vec3 keywordColor;
    
    Shader *textShader;
    Shader *rectShader;

    Font *textFont;

    Rect cmdBufferRect;
    Rect statusRectOne;
    Rect statusRectTwo;
} EditorVisuals;

void InitWindow();
void DrawEditor();

void WindowOpenFile(char *path);
void WindowFree();
void WindowResize(int width, int height);
void WindowUpdate();

// Editing
void WindowInsertCharacter(char c);
void WindowDeleteCharacter();
void WindowRedo();
void WindowUndo();

// Movement
void WindowMoveCursor(int colOffset, int rowOffset);
void WindowMoveStartOfLine();
void WindowMoveEndOfLine();
void WindowMoveStartOfWord();
void WindowMoveToNextSpace();
void WindowMoveToEmptyLine(int rowChange);

void WindowScroll(GLfloat amount);

// CommandBuffer
void WindowSetCommand(int command);
void WindowEvaluate();
void WindowChangeMode(int mode);

// Panel operations
void WindowClosePanel();
void WindowOpenPanel();
void WindowNextPanel();
void WindowSetMark();

int WindowFindString(char *query);
int WindowFindStringReverse(char *query);

void WindowCopy();
void WindowPaste();
#endif

