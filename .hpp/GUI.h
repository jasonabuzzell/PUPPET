#ifndef GUI_H
#define GUI_H
#pragma once

#include "glfw3.h"

class GUI {
    public:
        GUI();
        ~GUI();
    private:
        GLFWwindow* window;
};

#endif