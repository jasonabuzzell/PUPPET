#ifndef GAME_H
#define GAME_H
#pragma once

#include "json.hpp"
#include "XYZ.h"
#include "Character.h"
#include "Helper.h"
#include <fstream>
#include <string>

class Game
{
    public:
        Game();
        void getInput();
        int *getTime();
        int *getTimer();
        int choice(std::vector<std::string> allChoices, std::string print, int count);

        nlohmann::json moving(XYZ xyz, Character zero, nlohmann::json actions);
        nlohmann::json interact(XYZ xyz, Character zero, nlohmann::json actions);
        Strint printingInner(nlohmann::json catalog);
        nlohmann::json print(Character chara, nlohmann::json actions, Strint strint, std::string room);
        nlohmann::json printing(Character chara, nlohmann::json actions);
        void automate();
        nlohmann::json waiting(Character chara, nlohmann::json actions);
        void manualInner(int choice, std::ifstream *readme);
        void manual();
        nlohmann::json optionsInputType(nlohmann::json options);
        void options();
        void save(XYZ xyz, std::vector<Character> characters);
        nlohmann::json actions(XYZ xyz, Character chara, nlohmann::json actions);
        void singlePlayer(XYZ xyz, std::vector<Character> characters);
        void load(nlohmann::json xyzFile, nlohmann::json charactersFile);
        void newGame();
        void resumeGame();
        void singleplayerSelection();
        void menu();

    private:
        std::string input;
        std::string *inputType;
        std::string *strChoice;
        int *enumChoice;
        bool *exit;
        int *time;
};

#endif