#include "context.h"

// System libraies
#include <cstdio>

// Project libraries
#include "glew/glew.h"
#include "glfw/glfw3.h"

// Additional error check for initalization.
void APIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    printf(message);
}

bool context::init(int width, int height, const char* title)
{
    // Initialize GLFW
    int glfwStatus = glfwInit();
    if (glfwStatus == GLFW_FALSE)
    {
        printf("An error occurred with GLFW");
        return false;
    }

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    int glewStatus = glewInit();
    if (glewStatus != GLEW_OK)
    {
        printf("An error occurred with GLEW");
        return false;
    }

    // Print some diagnostics
    // Version, Renderer, Vendor, Shading Language Version, Error Check
    printf("OpenGL Version: %s\n", (const char*)glGetString(GL_VERSION));
    printf("Renderer: %s\n", (const char*)glGetString(GL_RENDERER));
    printf("Vendor: %s\n", (const char*)glGetString(GL_VENDOR));
    printf("GLSL: %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback(errorCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true);
#endif

    // Setup some defaults
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

    // Return true if successful.
    return true;
}

void context::tick()
{
    // Update GLFW
    glfwPollEvents();           // Update events, input, etc.
    glfwSwapBuffers(window);    // Update window.

    // Update delta time.
}

float context::time() const
{
    return (float)glfwGetTime();
}

void context::resetTime(float resetValue)
{
    glfwSetTime(resetValue);
}

void context::term()
{
    // Clean up Window
    glfwDestroyWindow(window);
    window = nullptr;

    // Clean up GLFW
    glfwTerminate();
}

void context::clear()
{
    // Clears the scren and replaces it with the
    // color specified during the init process.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool context::shouldClose() const
{
    return glfwWindowShouldClose(window);
}
