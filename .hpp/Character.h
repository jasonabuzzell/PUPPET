#ifndef CHARACTER_H
#define CHARACTER_H
#pragma once

#include "XYZ.h"
#include <string>
#include <vector>

class Character
{
    public:
        Character(XYZ xyz, std::string place, std::string part);
        void setLocation(std::string loc);
        void setPoint(std::string poi);
        std::string *getLocation();
        std::string *getPoint();
        float *getX();
        float *getY();
        float *getZ();
        std::vector<std::string> getInventory();
        bool getMoveFlag();
        bool getLookFlag();
        bool getAbleFlag();
        std::vector<std::string> possibleActions();
        void look(XYZ xyz);
        void printActions(XYZ xyz, std::vector<std::string> actions);
        std::vector<std::string> allActions(XYZ xyz);
        nlohmann::json possibleMoves(XYZ xyz);
        void printMoves(XYZ xyz, std::vector<std::string> moves);
        void move(std::string room, std::string point, 
                  std::vector<int> coords, int expTime, int *time);

    protected:
        std::string *location;
        std::string *point;
        float *x;
        float *y;
        float *z;
        std::vector<std::string> inventory;
        bool moveFlag;
        bool lookFlag;
        bool ableFlag;
    private:
        int decision;
};

#endif