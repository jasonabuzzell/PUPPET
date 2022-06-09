#ifndef GAME_H
#define GAME_H
#pragma once

#include "json.hpp"
#include "XYZ.h"
#include "Character.h"
#include "GUI.h"
#include "Helper.h"
#include <fstream>
#include <thread>
#include <string>

class Game
{
    public:
        Game();
        void getInput();
        int *getTime();
        int *getTimer();
        int choice(std::vector<std::string> allChoices, std::string print, int count);

        nlohmann::json moving(XYZ xyz, Character chara, nlohmann::json actions);

        nlohmann::json interact(XYZ xyz, Character chara, nlohmann::json actions);

        nlohmann::json waiting(Character chara, nlohmann::json actions);

        Strint printingInner(nlohmann::json catalog);
        nlohmann::json printing(Character chara, nlohmann::json actions);

        nlohmann::json automateActionsMove(XYZ xyz, Character chara, std::vector<std::string> parts, nlohmann::json actions);
        nlohmann::json automateActionsInteract(XYZ xyz, Character chara, std::vector<std::string> parts, nlohmann::json actions);
        nlohmann::json automateActions(XYZ xyz, std::vector<Character> charas, Character chara, nlohmann::json actions);
        void automateUpdate(std::vector<Character> charas, Character chara, std::string parameter);
        nlohmann::json automateEnable(nlohmann::json dict);
        void automate(Character chara);

        void manualInner(int choice, std::ifstream *readme);
        void manual();

        nlohmann::json optionsInputType(nlohmann::json options);
        nlohmann::json optionsAutosave(nlohmann::json options);
        nlohmann::json optionsGUI(nlohmann::json options);
        void options();

        void save(XYZ xyz, std::vector<Character> characters);

        nlohmann::json actionsMove(XYZ xyz, std::vector<Character> charas, Character chara, nlohmann::json actions);
        nlohmann::json actionsLook(XYZ xyz, std::vector<Character> charas, Character chara, nlohmann::json actions);
        nlohmann::json actionsInteract(XYZ xyz, std::vector<Character> charas, Character chara, nlohmann::json actions);
        nlohmann::json actionsWait(XYZ xyz, std::vector<Character> charas, Character chara, int counter, nlohmann::json actions);
        nlohmann::json actionsPrint(XYZ xyz, std::vector<Character> charas, Character chara, nlohmann::json actions);
        nlohmann::json actions(XYZ xyz, std::vector<Character> charas, nlohmann::json actions);

        void singlePlayer(nlohmann::json file);
        void newGame();
        void resumeGame();
        void singleplayerSelection();
        void menu();

    private:
        GUI gui;
        std::thread *threadGUI;
        std::string input;
        std::string *inputType;
        std::string *strChoice;
        int *enumChoice;
        int *time;
        bool *exit;
        bool autosave;
        bool *guiFlag;
};

#endif