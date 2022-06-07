#include "../.hpp/Item.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/json.hpp"
#include "../.hpp/Character.h"
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
    ifstream ifs(".json/items.json");
    point = json::parse(ifs)["Location"][roomName][itemName];
}

string Item::getPoint() {
    return point;
}

vector<float> Item::getCoords(XYZ xyz, Character zero) {
    if (onPerson) return zero.getCoords();
    else return xyz.listRooms()[*zero.getLocation()][point]["Coords"].get<vector<float>>(); 
    // can only access if no one is carrying the item.
}

string Item::getName() {
    return name;
}

// Not part of the Item class because it initializes the point of an item, which is needed to create a new item.
void addItem(string name, string newRoom, string newPoint) {
    ifstream ifs(".json/items.json");
    json item;
    json items = json::parse(ifs);
    ifs.close();

    item[name] = newPoint;
    if (!items["Location"].contains(newRoom)) items["Location"][newRoom] = item;
    else items["Location"][newRoom].update(item);

    ofstream ofs(".json/items.json");
    ofs << items;
    ofs.close();
}

void Item::update(string oldRoom, string newRoom, string newPoint, bool take) {
    ifstream ifs(".json/items.json");
    json items = json::parse(ifs);
    ifs.close();

    items["Location"][oldRoom].erase(name);
    if (!items["Location"].contains(newRoom)) items["Location"][newRoom] = json({}); // found this as just "{}"?
    items["Location"][newRoom][name] = newPoint;
    onPerson = take;

    ofstream ofs(".json/items.json");
    ofs << items;
    ofs.close();
}

json Item::use(json actions) {
    vector<string> allItems = {"Camera", "Chaff Grenade", "Phone", "Printer Carry", "Printer Pen", "Radar", "Sonar", "Exmit", "Ferroform", "Grenade", "Handgun", "Rifle", "Shotgun", "Spider Cage", "The Sinners", "The Thieves"};
    return actions;
}