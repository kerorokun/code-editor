#ifndef EDITOR_OPENGL
#define EDITOR_OPENGL

#include <glad/glad.h>
#include "editor_platform.h"

typedef struct {
    GLuint VAO;
    GLuint VBO;
} GLQuad;

void InitGL();
void GLDrawQuad(GLfloat xpos, GLfloat ypos, GLfloat z, GLfloat w, GLfloat h);

#endif

