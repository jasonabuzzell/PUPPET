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
        int *getTimer();
        void exitCheck();
        void menu();
        void options();
        void moving(XYZ xyz, Character zero);
        void investing(Character zero);
        void play();
        void main();

    private:
        std::string input;
        bool *exit;
        int *choice;
        int *time;
        int currentFile;
        int resume;
};

#endif