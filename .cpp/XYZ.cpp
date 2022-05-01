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

json XYZ::listRooms() {
    ifstream i("../.json/rooms.json");
    json rooms;
    i >> rooms;
    return rooms[to_string(config)];
}
