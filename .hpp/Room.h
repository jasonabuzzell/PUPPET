#ifndef ROOM_H
#define ROOM_H
#pragma once

#include <string>
#include <vector>

class XYZ; // Avoiding Circuluar Dependency
class Room
{
    public:
        Room(std::string title);
        std::string getName();
        std::vector<std::string> items(XYZ xyz);
    private:
        std::string name;
};

#endif