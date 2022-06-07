#include "../.hpp/json.hpp"
#include "../.hpp/XYZ.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
using namespace nlohmann;

XYZ::XYZ()
    : config(0)
{
    cout << "\nLOADING: [XYZ] ... \n";
    cout << "----------------------------\n";
}

int XYZ::getConfig() {
    return config;
}

void XYZ::setConfig(int state) {
    config = state;
}

json XYZ::listRooms() {
    ifstream ifs(".json/XYZ.json");
    json rooms = json::parse(ifs);
    return rooms[to_string(config)];
}

void XYZ::removeItem(string room, string item) {
    fstream fs(".json/XYZ.json");
    json rooms = json::parse(fs);
    json items = rooms[config][room]["Items"];
    items.erase(item);
    fs << rooms;
    fs.close();
}

json XYZ::getItems(string location) {
    ifstream ifs(".json/items.json");
    json items = json::parse(ifs)["Location"][location];
    return items;
}
