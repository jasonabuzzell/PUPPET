#ifndef ITEM_H
#define ITEM_H

#pragma once

#include "XYZ.h"
#include "Character.h"
#include <string>
#include <vector>

void addItem(std::string itemName, std::string room, std::string point);

class Item {
public:
    Item(std::string roomName, std::string itemName, bool take);
    std::string getPoint();
    std::vector<float> getCoords(XYZ xyz, Character zero);
    std::string getName();
    void setPerson(bool take);
    void add(std::string newRoom, std::string newPoint);
    void update(std::string oldRoom, std::string newRoom, std::string newPoint, bool take);
    nlohmann::json use(nlohmann::json actions);

private:
    std::string point;
    std::vector<float> coords;
    std::string name;
    bool onPerson;
};

#endif