#ifndef GAME_H
#define GAME_H
#pragma once

#include "json.hpp"
#include "XYZ.h"
#include "Character.h"
#include <string>

class Game
{
    public:
        Game();
        void choiceReset();
        void getInput();
        int *getTimer();
        void exitCheck();
        void toolsMenu(nlohmann::json catalog);
        void weaponsMenu(nlohmann::json catalog);
        void menu();
        nlohmann::json optionsInputType(nlohmann::json options);
        void options();
        void manual();
        void moving(XYZ xyz, Character zero);
        void investing(Character zero);
        void play();
        void main();

    private:
        std::string input;
        std::string *inputType;
        std::string *strChoice;
        int *enumChoice;
        bool *exit;
        int *time;
        int currentFile;
        int resume;
};

#endif