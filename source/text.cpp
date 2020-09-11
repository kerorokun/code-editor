#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>

#include "global.h"
#include "text.h"

Font *
MakeFont(char *font, int size)
{
    FT_Library ft;
    // TODO: Handle error checking
    FT_Init_FreeType(&ft);
    
    // TODO: Handle error checking
    FT_Face face;
    
    char *path = (char *)malloc(1 + strlen("fonts/") + strlen(font) + strlen(".ttf"));
    
#ifdef _WIN32
    strcpy_s(path, 1 + strlen("fonts/") + strlen(font) + strlen(".ttf"), "fonts/");
    strcat_s(path, 1 + strlen("fonts/") + strlen(font) + strlen(".ttf"), font);
    strcat_s(path, 1 + strlen("fonts/") + strlen(font) + strlen(".ttf"), ".ttf");
#elif __linux__
    strcpy(path, "fonts/");
    strcat(path, font);
    strcat(path, ".ttf");
#endif
    
    FT_New_Face(ft, path, 0, &face);
    FT_Set_Pixel_Sizes(face, 0, size);
    
    // Free the string
    free(path);

    // Allocate the font
    Font *fontResult = new Font();//(Font *) malloc(sizeof(Font));
    
    // Disable byte-alignment restriction for loading fonts
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "ERROR::FREETYPE: Failed to load Glyph\n");
            continue;
        }
        
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Now store character for later use
        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        (*fontResult).characters.emplace(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    fontResult->characterWidth = fontResult->characters['w'].Size.x;
    fontResult->characterHeight = fontResult->characters['{'].Size.y;
    fontResult->characterAdvance = fontResult->characters['w'].Advance;
    return fontResult;
}
