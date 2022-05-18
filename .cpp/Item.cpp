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

Item::Item(XYZ xyz, string roomName, string itemName, bool take)
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
    else return xyz.listRooms()[*zero.getLocation()][point]["coords"].get<vector<float>>(); 
    // can only access if no one is carrying the item.
}

string Item::getName() {
    return name;
}

void onPerson(bool take) {

}

void Item::update(string oldRoom, string newRoom, string newPoint, bool take) {
    ifstream ifs(".json/items.json");
    json items = json::parse(ifs);
    ifs.close();

    items["Location"][oldRoom].erase(name);
    if (!items["Location"].contains(newRoom)) items["Location"][newRoom] = {};
    items["Location"][newRoom][name] = newPoint;
    onPerson = take;

    ofstream ofs(".json/items.json");
    ofs << items;
    ofs.close();
}

void Item::selector() {
    if (name == "Phone") {
        Phone phone;
        phone.use();
    }
    // Add in other items (Also, weapons inherit from weapon class)
    return;
}

Phone::Phone() {}

void Phone::use() {
    cout << "Beep boop, I'm a phone.\n";
}