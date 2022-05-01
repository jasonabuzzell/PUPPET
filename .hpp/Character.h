#ifndef CHARACTER_H
#define CHARACTER_H
#pragma once

#include "XYZ.h"
#include <string>
#include <vector>

class Character
{
    public:
        Character(std::string place);
        std::string getLocation();
        int *getTimer();
        virtual std::vector<std::string> possibleActions();
        void printActions(XYZ xyz, std::vector<std::string> actions);
        std::vector<std::string> allActions(XYZ xyz);
        std::vector<std::string> possibleMoves(XYZ xyz);
        void printMoves(XYZ xyz, std::vector<std::string> moves);
        int move(XYZ xyz, std::string room);
    protected:
        std::string* location;
        int *timer;
        bool moveFlag;
        bool lookFlag;
        bool ableFlag;
    private:
        int decision;
};

class User: public Character
{
    public:
        using Character::Character;
        std::vector<std::string> possibleActions();
        void look(XYZ xyz);
};

#endif