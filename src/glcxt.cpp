#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <thread>
#include <glm/glm.hpp>

#include "glcxt.hpp"
#include "myshaders.hpp"

namespace mbwk {

static int preferredWidth = 800, preferredHeight = 600;

static auto
createWindow(int width, int height, std::string title) -> GLFWwindow*
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    preferredWidth = width;
    preferredHeight = height;

    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

static GLsizei elec = 3;
static float alphamod = 1.f;
static bool fullscreen = false;

static void
fbSizeCallback(GLFWwindow *window, int width, int height)
{
    auto prevGlCxt = glfwGetCurrentContext();
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    glfwMakeContextCurrent(prevGlCxt);
}

static GLFWmonitor*
getMonitor(GLFWwindow *window)
{
    auto monitor = glfwGetWindowMonitor(window);
    if (!monitor) {
        monitor = glfwGetPrimaryMonitor();
    }
    return monitor;
}

static void
toggleFullScreen(GLFWwindow *window)
{
    if (fullscreen) {
        auto vidmode = glfwGetVideoMode(getMonitor(window));
        int halfMonWidth = vidmode->width / 2, halfMonHeight = vidmode->height / 2,
            halfWinWidth = preferredWidth / 2, halfWinHeight = preferredHeight / 2;
        glfwSetWindowPos(window, halfMonWidth - halfWinWidth, halfMonHeight - halfWinHeight);
        glfwSetWindowSize(window, preferredWidth, preferredHeight);
    } else {
        std::cerr << "setting size from vidmode\n";
        auto vidmode = glfwGetVideoMode(getMonitor(window));
        glfwSetWindowPos(window, 0, 0);
        glfwSetWindowSize(window, vidmode->width, vidmode->height);
    }
    fullscreen = !fullscreen;
}

auto static selectElements() -> void;

static auto
keyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mods) -> void
{
    if (action == GLFW_PRESS) return;

    std::cout << "キー「" << key << "」を押した　";

    switch (key) {
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            std::cout << "（）";
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_SPACE:
            if (elec == 3) {
                elec = 6;
            } else {
                elec = 3;
            }
            selectElements();
            break;
        case GLFW_KEY_T:
            if (alphamod > 0.5f) {
                alphamod = 0.f;
            } else {
                alphamod = 1.f;
            }
            break;
        case GLFW_KEY_F:
            toggleFullScreen(window);
            break;
    }
    std::cout << '\n';
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
     0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, 1.0f, 1.0f, 0.0f
};

GLuint tri_elements[] = {
    0, 1, 2
};

GLuint sqr_elements[] = {
    0, 1, 3,
    0, 3, 4
};

auto static
selectElements() -> void
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
    glfwSetFramebufferSizeCallback(mainWindow, fbSizeCallback);

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

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(mainWindow)) {
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        float alval =  1.0 - (alphamod * (std::sin(time * 4.0f) + 1.0f) / 2.0f);

        glUniform1f(unialpha, alval);

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

