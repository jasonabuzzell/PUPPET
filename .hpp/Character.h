#ifndef CHARACTER_H
#define CHARACTER_H
#pragma once

#include "XYZ.h"
#include <string>
#include <vector>

class Character
{
    public:
        Character(XYZ xyz, std::string title, std::string place, std::string part, bool use);
        void setParameters(std::vector<std::string> position, std::vector<float> coords, std::vector<bool> flags);
        void setLocation(std::string loc);
        void setPoint(std::string poi);
        void setCoords(std::vector<float> coords);
        std::string getName();
        std::string *getLocation();
        std::string *getPoint();
        bool getUser();
        float *getX();
        float *getY();
        float *getZ();
        std::vector<float> getCoords();
        bool getMoveFlag();
        bool getLookFlag();
        bool getAbleFlag();
        std::vector<std::string> possibleActions();
        nlohmann::json look(XYZ xyz, nlohmann::json actions);
        void printActions(XYZ xyz, std::vector<std::string> actions);
        std::vector<std::string> allActions(XYZ xyz);
        int distance(std::vector<float> a, std::vector<float> b);
        nlohmann::json distanceTime(std::vector<float> a, std::vector<float> b);
        nlohmann::json possibleMoves(XYZ xyz);
        void printMoves(XYZ xyz, std::vector<std::string> moves);
        void move(std::vector<float> curCoords, std::vector<float> expCoords, int curTime, int expTime);
        nlohmann::json compareCoords(XYZ xyz, nlohmann::json rooms, std::vector<std::string> points, std::string room, std::string part);

    private:
        std::string name;
        std::string *location;
        std::string *point;
        float *x;
        float *y;
        float *z;
        bool moveFlag;
        bool lookFlag;
        bool ableFlag;
        bool user;
};

#endif