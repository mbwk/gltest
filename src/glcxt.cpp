#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <thread>
#include <cmath>

#include "glcxt.hpp"
#include "myshaders.hpp"

namespace mbwk {

static auto
createWindow(int width, int height, std::string title) -> GLFWwindow*
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

static auto
keyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mods) -> void
{
    switch (key) {
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
    }
}

GlContext::GlContext()
{
    glfwInit();
}

GlContext::~GlContext()
{
    glfwTerminate();
}

auto static
loadShaders() -> GLuint
{
    GLuint shaderProgram = glCreateProgram();

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    auto vsrc = vertsrc();
    glShaderSource(vertShader, 1, &vsrc, NULL);
    glCompileShader(vertShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    auto fsrc = fragsrc();
    glShaderSource(fragShader, 1, &fsrc, NULL);
    glCompileShader(fragShader);

    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);

    glBindFragDataLocation(shaderProgram, 0, "outColor");

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    return shaderProgram;
}

GLfloat vertices[] = {
    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 1.0f, 1.0f, 0.0f
};

GLuint tri_elements[] = {
    0, 1, 2
};

GLuint sqr_elements[] = {
    0, 1, 3,
    1, 3, 4
};

static GLsizei elec = 3;

auto static
selectElements()
{
    switch (elec) {
        case 6:
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sqr_elements), sqr_elements, GL_STATIC_DRAW);
            break;
        case 3:
        default:
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tri_elements), tri_elements, GL_STATIC_DRAW);
            break;
    }


}

auto static
mainWindow() -> void
{
    auto mainWindow = createWindow(800, 600, "main window");
    glfwMakeContextCurrent(mainWindow);
    glewExperimental = GL_TRUE;
    glewInit();

    glfwSetKeyCallback(mainWindow, keyCallbackFunc);

    GLuint vbo, vao, ebo, tex;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    selectElements();

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLuint shaderProgram = loadShaders();

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    GLfloat unialpha = glGetUniformLocation(shaderProgram, "alpha");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    auto t_start = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(mainWindow)) {
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        float alval = (std::sin(time * 4.0f) + 1.0f) / 2.0f;

        glUniform1f(unialpha, alval);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, elec, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }

    glfwDestroyWindow(mainWindow);
}

auto GlContext::run() -> void
{
    mainWindow();
}

}

