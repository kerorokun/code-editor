#include "gl.h"

#include <string.h>
#include "global.h"

static GLQuad rectQuad;

void
InitGL()
{
    // Quads
    glGenVertexArrays(1, &(rectQuad.VAO));
    glGenBuffers(1, &(rectQuad.VBO));
    glBindVertexArray(rectQuad.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectQuad.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 3, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void
GLDrawQuad(GLfloat xpos, GLfloat ypos, GLfloat z, GLfloat w, GLfloat h)
{
    glBindVertexArray(rectQuad.VAO);
    
    GLfloat vertices[6][3] = {
        { xpos,     ypos + h,   z },            
        { xpos,     ypos,       z },
        { xpos + w, ypos,       z },
        
        { xpos,     ypos + h,   z },
        { xpos + w, ypos,       z },
        { xpos + w, ypos + h,   z }           
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, rectQuad.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}



