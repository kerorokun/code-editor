#ifndef PANEL_H
#define PANEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#include "file.h"
#include "text.h"
#include "array.h"
#include "gl.h"

// NOTE: Handles cursor, and displaying buffer
typedef struct {
    BufferFile *visitedFile;
    char *statusBar;
    
    // Line information
    Array *linesArr;
    i16 maxCharsInLine;
    
    int cursorIndex;
    int markIndex;
    
    int startRow;
    int savedRowIndex;
    bool linesSynced;
    
    /* Visuals */
    GLfloat yOffset;
    GLfloat startX;
    GLfloat startY;
    GLuint width;
    GLuint height;
} Panel;


typedef struct {
    f32 yOffset;
    Rect panelRect;
    Rect statusRect;
} PanelVisuals;

void PanelInit(Panel *panel, Font *font, BufferFile *file, GLuint startX, GLuint startY, GLuint width, GLuint height);
void PanelMoveCursor(Panel *panel, int colChange, int rowChange);
void PanelMoveStartOfLine(Panel *panel);
void PanelMoveStartOfWord(Panel *panel);
void PanelMoveToNextSpace(Panel *panel);
void PanelMoveEndOfLine(Panel *panel);
void PanelInsertCharacter(Panel *panel, char c, bool editHistory);
void PanelDeleteCharacter(Panel *panel, bool editHistory);
void PanelUndo(Panel *panel);
void PanelRedo(Panel *panel);

void PanelAdjustViewspace(Panel *panel);

void PanelSetMark(Panel *panel);

void PanelCopy(Panel *panel);
void PanelPaste(Panel *panel);
void PanelFree(Panel *panel);

void PanelUpdate(Panel *panel);

void PanelMoveToEmptyLine(Panel *panel, int rowChange);

#endif

