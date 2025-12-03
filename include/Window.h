#ifndef WINDOW_H
#define WINDOW_H
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
using std::cout; using std::endl;
#include<string>
using std::string;

struct Window {
    unsigned width, height;
    string title;
    GLFWwindow *window;
    Window(unsigned width = 800, unsigned height = 600, const string &title = "project") : width(width), height(height), title(title)
    {
        if (!glfwInit()) {
            cout << "glfw init failed" << endl;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (!window) {
            cout << "window create failed" << endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            cout << "glad load failed" << endl;
            glfwTerminate();
        }
        glViewport(0, 0, width, height);
    }
    void updateWH(unsigned width, unsigned height)
    {
        this->width = width;
        this->height = height;
        glViewport(0, 0, width, height);
    }
    void updateTitle(const string &title)
    {
        this->title = title;
        glfwSetWindowTitle(window, title.c_str());
    }
    // 鼠标中键回调函数，切换将鼠标锁定在窗口之内
    void lockCursor()
    {
        static bool isCursorLocked = false;
        if (!isCursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isCursorLocked = true;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isCursorLocked = false;
        }

    }

    ~Window()
    {
        glfwTerminate();
    }

};

#endif