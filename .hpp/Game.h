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

        void moving(XYZ xyz, Character zero);
        void interact(XYZ xyz, Character zero);
        Strint printingInner(nlohmann::json catalog);
        void print(Character chara, nlohmann::json actions, Strint strint, std::string room);
        void printing(Character chara);
        void automate();
        void waiting();
        void manualInner(int choice, std::ifstream *readme);
        void manual();
        nlohmann::json optionsInputType(nlohmann::json options);
        void options();
        void save(XYZ xyz, std::vector<Character> characters);
        void actions(XYZ xyz, Character chara);
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