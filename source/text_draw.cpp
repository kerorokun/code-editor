#include "text_draw.h"


#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text.h"
#include "global.h"

#include <stdio.h>

// Keep it filescope 
static GLQuad textQuad;

void
InitDraw()
{
    // Text
    glGenVertexArrays(1, &(textQuad.VAO));
    glGenBuffers(1, &(textQuad.VBO));
    glBindVertexArray(textQuad.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, textQuad.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 8, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//== Assumes that the text shader is currently being used
void
DrawChar(Font *font, char c,
         GLfloat x, GLfloat y, GLfloat z,
         vec3 color)
{
    Character ch = font->characters[c];
    GLfloat w = ch.Size.x;
    GLfloat h = ch.Size.y;
    
    // Calculate the vertices
    // Position, Color, texCoords
    GLfloat vertices[6][8] = {
        { x,     y + h, z,  color.x, color.y, color.z,  0.0, 0.0 },            
        { x,     y,     z,  color.x, color.y, color.z,  0.0, 1.0 },
        { x + w, y,     z,  color.x, color.y, color.z,  1.0, 1.0 },
        
        { x,     y + h, z,  color.x, color.y, color.z,  0.0, 0.0 },
        { x + w, y,     z,  color.x, color.y, color.z,  1.0, 1.0 },
        { x + w, y + h, z,  color.x, color.y, color.z,  1.0, 0.0 }           
    };
    
    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, textQuad.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void
DrawGapBufferLine(Font *font,
                  GapBuffer *buffer,
                  int index,
                  int lineStart, int lineLength,
                  GLfloat x, GLfloat y, GLfloat z,
                  vec3 color)
{
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textQuad.VAO);

    char currChar;

    for (int i = index; i < buffer->preLength + buffer->postLength && lineStart < lineLength; i++, lineStart++) {
        if (i < buffer->preLength) {
            currChar = *(buffer->buffer + i);
        } else {
            currChar = *(buffer->gapEnd + i - buffer->preLength);
        }

        //====== Get glyph
        Character ch = font->characters[currChar];
        GLfloat xpos = x + ch.Bearing.x;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y);
        GLfloat w = ch.Size.x;
        GLfloat h = ch.Size.y;
        
        
        //===== Test for end (word wrapping)
        if (currChar == '\n') {
            break;
        }
        
        DrawChar(font, currChar, xpos, ypos, z, color);
        
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6);
    }
}

void
DrawText(Font *font, char *text,
         GLfloat x, GLfloat y, GLfloat z,
         vec3 color)
{
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textQuad.VAO);

    for (int i = 0; i < strlen(text); i++) {
        char currChar = *(text + i);

        //==== Get glyph
        Character ch = font->characters[currChar];
        GLfloat xpos = x + ch.Bearing.x;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y);
        GLfloat w = ch.Size.x;
        GLfloat h = ch.Size.y;

        DrawChar(font, currChar, xpos, ypos, z, color);

        x += (ch.Advance >> 6);
    }
}
