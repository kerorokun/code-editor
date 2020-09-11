#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdlib.h>

#if __linux__
#include <pthread.h>
#elif _WIN32
#include <windows.h>
#endif

#include "util.h"
#include "shader.h"
#include "global.h"
#include "buffer.h"
#include "window.h"
#include "array.h"
#include "editor_platform.h"
#include "text_draw.h"

#define SINGLE_THREAD 1

/* TODO: Figure out what I want to do with this */
GLuint WIDTH = 1280;
GLuint HEIGHT = 720;

GLfloat LINE_SPACING = 1.15f;

bool WINDOW_RESIZED = false;
bool SHOULD_RENDER = true;

GLuint FONT_SIZE = 16;

static GLFWwindow *window;

static void 
KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    
    if ((key == GLFW_KEY_TAB) &&
        (action == GLFW_PRESS || action == GLFW_REPEAT) &&
        (mods == GLFW_MOD_CONTROL)) {
        WindowNextPanel();
    }
    
    if ((key == GLFW_KEY_Z) &&
        (action == GLFW_PRESS || action == GLFW_REPEAT) &&
        (mods == GLFW_MOD_CONTROL)) {
        WindowUndo();
    }
    
    if ((key == GLFW_KEY_Y) &&
        (action == GLFW_PRESS || action == GLFW_REPEAT) &&
        (mods == GLFW_MOD_CONTROL)) {
        WindowRedo();
    }
    
    if ((key == GLFW_KEY_SPACE) &&
        (action == GLFW_PRESS || action == GLFW_REPEAT) &&
        (mods == GLFW_MOD_CONTROL)) {
        WindowSetMark();
    }
    
    if ((key == GLFW_KEY_C) &&
        (action == GLFW_PRESS || action == GLFW_REPEAT) && 
        (mods == GLFW_MOD_CONTROL)) {
        WindowCopy();
    }
    
    if ((key == GLFW_KEY_V) &&
        (action == GLFW_PRESS || action == GLFW_REPEAT) && 
        (mods == GLFW_MOD_CONTROL)) {
        WindowPaste();
    }
    
    if ((key == GLFW_KEY_F) &&
        (action == GLFW_PRESS || action == GLFW_REPEAT) &&
        (mods == GLFW_MOD_CONTROL)) {
        WindowChangeMode(COMMAND_BUFFER_MODE);
        WindowSetCommand(FIND_CMD);
    }
    
    if ((key == GLFW_KEY_R) &&
        (action == GLFW_PRESS || action == GLFW_REPEAT) &&
        (mods == GLFW_MOD_CONTROL)) {
        WindowChangeMode(COMMAND_BUFFER_MODE);
        WindowSetCommand(RFIND_CMD);
    }
    
    if (key == GLFW_KEY_ENTER && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        WindowEvaluate();
    }
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
        WindowMoveStartOfLine();
        
    }
    if (key == GLFW_KEY_END && action == GLFW_PRESS) {
        WindowMoveEndOfLine();
    }
    if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
         WindowDeleteCharacter();
    }
    if (key == GLFW_KEY_TAB && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS) && (mods == GLFW_MOD_CONTROL)) {
        WindowChangeMode(COMMAND_BUFFER_MODE);
        WindowSetCommand(SAVE_CMD);
        WindowEvaluate();
    }
    if (key == GLFW_KEY_O && 
        (action == GLFW_PRESS) && 
        (mods == GLFW_MOD_CONTROL)) {
        WindowChangeMode(COMMAND_BUFFER_MODE);
        WindowSetCommand(OPEN_CMD);
    }
    if (key == GLFW_KEY_LEFT && 
        (action == GLFW_PRESS || action == GLFW_REPEAT) && 
        (mods == GLFW_MOD_CONTROL)) {
        WindowMoveStartOfWord();
    } else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        WindowMoveCursor(-1, 0);
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT) && (mods == GLFW_MOD_CONTROL)) {
        WindowMoveToNextSpace();
    } else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        WindowMoveCursor(1, 0);
    }
    
    if (key == GLFW_KEY_UP && 
        (action == GLFW_PRESS || action == GLFW_REPEAT) && 
        (mods == GLFW_MOD_CONTROL)) {
        WindowMoveToEmptyLine(-1);
    } else if (key == GLFW_KEY_UP && 
               (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        WindowMoveCursor(0, -1);
    }
    
    
    if (key == GLFW_KEY_DOWN && 
        (action == GLFW_PRESS || action == GLFW_REPEAT) && 
        (mods == GLFW_MOD_CONTROL)) {
        WindowMoveToEmptyLine(1);
    } else if (key == GLFW_KEY_DOWN && 
               (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        WindowMoveCursor(0, 1);
    }

    if (key == GLFW_KEY_1 && 
        (action == GLFW_PRESS || action == GLFW_REPEAT) && 
        (mods == GLFW_MOD_CONTROL)) {
        WindowClosePanel();
    } 
        
    
    SHOULD_RENDER = true;
}

static void
FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    
    WINDOW_RESIZED = true;
    SHOULD_RENDER = true;
}
static void
CharCallback(GLFWwindow *window, unsigned int codepoint, int mods)
{
    char input[4] = "\0\0\0";
    char *output;
    code_to_utf8(input, codepoint);
    
    if (!(input[0] == ' ' && mods == GLFW_MOD_CONTROL)) {
        // TODO: Figure out how to use UTF-8 for text
        WindowInsertCharacter(input[0]);
    }
    SHOULD_RENDER = true;
}

static void
ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    // WindowScroll((GLfloat)-yoffset);
    // SHOULD_RENDER = true;
}


#if _WIN32
DWORD WINAPI
Render(void *data)
{
    GLFWwindow *window = (GLFWwindow *)data;
    // Set current context to be the window
    glfwMakeContextCurrent(window);
    
    // Setup OpenGL
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    // Set viewport
    glViewport(0, 0, WIDTH, HEIGHT);
    
    // Set OpenGL options
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Main loop ====================================================================
    while (!glfwWindowShouldClose(window)) {
        if (SHOULD_RENDER) {
            // Clear the colorbuffer
            glClearColor(0.0667f, 0.0625f, 0.0781f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glEnable(GL_DEPTH_TEST);
            
            if (WINDOW_RESIZED) {
                glViewport(0, 0, WIDTH, HEIGHT);
                WindowResize(WIDTH, HEIGHT);
                WINDOW_RESIZED = false;
            }
            
            // WindowRender();
            // DrawWindow();
            
            // Swap the buffers
            glfwSwapBuffers(window);
            
            Sleep(32);
            SHOULD_RENDER = false;
        }
    }
    
    return 0;
}
#endif

// TODO: Render for pthread

int
InitEditor()
{

    
    // Init library
    if (!glfwInit()) { return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    WIDTH = 1280;
    HEIGHT = 720;
    
    window = glfwCreateWindow(WIDTH, HEIGHT, "Word Processor", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // Set callbacks
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharModsCallback(window, CharCallback);
    glfwSetScrollCallback(window, ScrollCallback);

#if SINGLE_THREAD
    // Set current context to be the window
    glfwMakeContextCurrent(window);
    
    // Setup OpenGL
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    // Set viewport
    glViewport(0, 0, WIDTH, HEIGHT);
    
    // Set OpenGL options
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    InitWindow();
    InitGL();
    InitDraw();
    
    //LoadFont("LiberationMono-Regular", FONT_SIZE);
#else
    
#ifdef __linux__
    pthread_t renderTid;
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_create(&renderTid, &attr, Render, (void *)window);
#elif _WIN32
    HANDLE renderId = CreateThread(NULL, 0, Render, (void *)window, 0, NULL);
#endif
#endif
    

}

void
MainLoop()
{
    // Main loop ====================================================================
    while (!glfwWindowShouldClose(window)) {
        // Check and call events
        glfwPollEvents();
        
#if SINGLE_THREAD
        if (SHOULD_RENDER) {
            // Clear the colorbuffer
            glClearColor(0.0667f, 0.0625f, 0.0781f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glEnable(GL_DEPTH_TEST);
            
            if (WINDOW_RESIZED) {
                glViewport(0, 0, WIDTH, HEIGHT);
                WindowResize(WIDTH, HEIGHT);
                WINDOW_RESIZED = false;
            }
            
            WindowUpdate();
            DrawEditor();
            
            // Swap the buffers
            glfwSwapBuffers(window);
            
            Sleep(16);
            SHOULD_RENDER = false;
        }
#endif
    }
    
#if SINGLE_THREAD
#else
#ifdef __linux__
    pthread_join(renderTid, NULL);
#elif _WIN32
    WaitForSingleObject(renderId, INFINITE);
#endif
    
#endif    
}

void
ExitEditor()
{
    //WindowFree();
    //delete();
    
    glfwTerminate();
}

int
main()
{
    if (InitEditor() < 0) { return -1; }
    MainLoop();
    ExitEditor();

    return 0;
}
