#ifndef TEXT_H
#define TEXT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string.h>
#include <map>

#include "editor_platform.h"

typedef struct {
    GLuint     TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;       // Size of glyph
    glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
    GLuint     Advance;    // Offset to advance to next glyph
} Character;

typedef struct {
    std::map<GLchar, Character> characters;
    i16 characterHeight;
    i16 characterWidth;
    i16 characterAdvance;
} Font;

Font *MakeFont(char *font, int size);
#endif
