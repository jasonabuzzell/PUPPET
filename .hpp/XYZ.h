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
        Room getRoom();
        nlohmann::json listRooms();

    private:
        int config;
};

#endif