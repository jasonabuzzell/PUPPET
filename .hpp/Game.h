#ifndef GAME_H
#define GAME_H
#pragma once

#include "json.hpp"
#include "XYZ.h"
#include "Character.h"
#include <fstream>
#include <string>

class Game
{
    public:
        Game();
        void convertInput(nlohmann::json allChoices);
        void convertStrInt();
        void choiceReset();
        void getInput();
        int *getTime();
        int *getTimer();
        void exitCheck();
        void print(std::string key, int value);
        void printingInner(Character zero, nlohmann::json catalog);
        void printing(Character zero);
        nlohmann::json optionsInputType(nlohmann::json options);
        void options();
        void manualInner(int choice, std::ifstream *readme);
        void manual();
        void moving(XYZ xyz, Character zero);
        void use(XYZ xyz, Character zero);
        void waiting();
        void singleplayerSelection();
        void save(XYZ xyz, std::vector<Character> characters);
        void load(nlohmann::json xyzFile, nlohmann::json charactersFile);
        void newGame();
        void resumeGame();
        void singlePlayer(XYZ xyz, std::vector<Character> characters);
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