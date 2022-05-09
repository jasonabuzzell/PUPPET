#ifndef XYZ_H
#define XYZ_H
#pragma once

#include "json.hpp"
#include "Room.h"
#include <vector>

class XYZ
{
    public:
        XYZ();
        int getConfig();
        void setConfig(int state);
        nlohmann::json listRooms();

    private:
        int config;
};

#endif