#include "../.hpp/json.hpp"
#include "../.hpp/XYZ.h"
#include "../.hpp/Room.h"
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
    ifstream ifs(".json/rooms.json");
    json rooms = json::parse(ifs);
    return rooms[to_string(config)];
}
