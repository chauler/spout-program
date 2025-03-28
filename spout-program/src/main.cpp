#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <iostream>
#include "escapi\escapi.h"
#include "App.h"
#include "tracy/public/tracy/Tracy.hpp"
#include <Renderer.h>

int Main(int, char**);
GLFWwindow* InitWindow();

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void cleanup(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

#if defined(DEBUG)
    int main(int argc, char** argv) {
        return Main(argc, argv);
    }
#elif defined(RELEASE)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdLine, int cmdShow) {
        return Main(__argc, __argv);
    }
#endif


int Main(int, char**)
{
    
    GLFWwindow* window = InitWindow();
    if (window == nullptr) {
        return 1;
    }

    App app{window};
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        app.RunLogic();
        app.DrawGUI();
        glfwSwapBuffers(window);
        FrameMark;
    }
    cleanup(window);
    return 0;
}

GLFWwindow* InitWindow() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return nullptr;

    //We rely on features only present in modern OpenGL anyway, so just go ahead and assume that version.
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Spout Effects", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Error creating GLFW window" << std::endl;
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        std::cerr << "glewInit() error!" << std::endl;
        return nullptr;
    }
    glfwSwapInterval(1); // Enable vsync
    GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    setupESCAPI();
    return window;
}