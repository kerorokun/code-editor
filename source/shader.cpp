#include <glad/glad.h>
#include <stdio.h>

#include "file.h"
#include "shader.h"

void
CheckCompileErrors(unsigned int shader, const char *type)
{
    int success;
    char infoLog[1024];
    
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n", type, infoLog);
        }
    }
}

GLint
CreateShader(const char *vertexPath, const char *fragmentPath)
{
    /* ID that will be mapped to the part of the GPU saving the shader*/
    GLint ID;
    
    // Open vertex shader
    char *vsCode = ReadEntireFile(vertexPath);
    
    // Open fragment shader
    char *fsCode = ReadEntireFile(fragmentPath);
    
    /* Compile shaders */
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vsCode, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fsCode, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");
    
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");
    
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return ID;
}

void
InitShader(Shader *shader, const char *vsPath, const char *fsPath)
{
    GLuint vs, fs, program;
    
    char *vsCode = ReadEntireFile(vsPath);
    char *fsCode = ReadEntireFile(fsPath);
    
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsCode, NULL);
    glCompileShader(vs);
    CheckCompileErrors(vs, "VERTEX");
    
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsCode, NULL);
    glCompileShader(fs);
    CheckCompileErrors(fs, "FRAGMENT");
    
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    CheckCompileErrors(program, "PROGRAM");
    
    shader->fShader = fs;
    shader->vShader = vs;
    shader->program = program;
}

// TODO: Check if necessary
void
FreeShader(Shader *shader)
{
    glDeleteShader(shader->vShader);
    glDeleteShader(shader->fShader);
}
