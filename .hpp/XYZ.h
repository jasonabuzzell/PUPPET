#ifndef XYZ_H
#define XYZ_H
#pragma once

#include "Room.h"
#include "json.hpp"
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