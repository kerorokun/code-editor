#ifndef EDITOR_DRAW_H
#define EDITOR_DRAW_H

#include <glad/glad.h>
#include "gl.h"
#include "buffer.h"
#include "text.h"
#include "editor_platform.h"
#include "shader.h"

typedef struct {
    int rowTextIndex;
    int leftMargin;
    int numChars;
} GlyphRow;

void InitDraw();
void PrepRect();
void DrawChar(Font *font, char c, GLfloat x, GLfloat y, GLfloat z, vec3 color);
void DrawText(Font *font, char *text, GLfloat x, GLfloat y, GLfloat z, vec3 color);
void DrawGapBufferLine(Font *font,
                       GapBuffer *buffer,
                       int index,
                       int lineStart, int lineLength,
                       GLfloat x, GLfloat y, GLfloat z,
                       vec3 color);
#endif
