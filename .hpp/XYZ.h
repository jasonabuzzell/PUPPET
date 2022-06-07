#ifndef XYZ_H
#define XYZ_H
#pragma once

#include "json.hpp"
#include <string>
#include <vector>

class XYZ
{
    public:
        XYZ();
        int getConfig();
        void setConfig(int state);
        nlohmann::json listRooms();
        void removeItem(std::string room, std::string item);
        nlohmann::json getItems(std::string location);
    private:
        int config;
};

#endif