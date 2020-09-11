#ifndef SHADER_H
#define SHADER_H


typedef struct {
    GLuint vShader;
    GLuint fShader;
    GLuint program;
    
    bool backfaceCull;
} Shader;

void CheckCompileErrors(unsigned int shader, const char *type);
GLint CreateShader(const char *vertexPath, const char *fragmentPath);
void InitShader(Shader *shader, const char *vsPath, const char *fsPath);
void FreeShader(Shader *shader);

#endif
