#ifndef GAME_H
#define GAME_H
#pragma once

#include "../.hpp/XYZ.h"
#include "../.hpp/Character.h"
#include <string>

class Game
{
    public:
        Game();
        void getInput();
        void exitCheck();
        void menu();
        void options();
        void moving(XYZ xyz, User zero);
        void play();
        void main();

    private:
        std::string input;
        bool *exit;
        int *choice;
        int currentFile;
        int resume;
};

#endif