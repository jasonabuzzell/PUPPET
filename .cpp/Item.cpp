#include "../.hpp/Item.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/json.hpp"
#include "../.hpp/Character.h"
#include "../.hpp/Helper.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace nlohmann;

Item::Item(string roomName, string itemName, bool take)
: onPerson(take), name(itemName)
{
    json items = jsonLoad("items");
    point = items["Location"][roomName][itemName];
}

string Item::getPoint() {
    return point;
}

string Item::getName() {
    return name;
}

void Item::setPerson(bool take) {
    onPerson = take;
}

// Not part of the Item class because it initializes the point of an item, which is needed to create a new item.
void addItem(string name, string newRoom, string newPoint) {
    json items = jsonLoad("items");
    json item;

    item[name] = newPoint;
    if (!items["Location"].contains(newRoom)) items["Location"][newRoom] = item;
    else items["Location"][newRoom].update(item);

    jsonSave(items, "items");
}

void Item::update(string oldRoom, string newRoom, string newPoint, bool take) {
    json items = jsonLoad("items");

    items["Location"][oldRoom].erase(name);
    if (!items["Location"].contains(newRoom)) items["Location"][newRoom] = json({});
    items["Location"][newRoom][name] = newPoint;
    onPerson = take;

    jsonSave(items, "items");
}

json Item::use(json actions) {
    vector<string> allItems = {"Camera", "Chaff Grenade", "Phone", "Printer Carry", "Printer Pen", "Radar", "Sonar", "Exmit", "Ferroform", "Grenade", "Handgun", "Rifle", "Shotgun", "Spider Cage", "The Sinners", "The Thieves"};

    // Adding more

    return actions;
}