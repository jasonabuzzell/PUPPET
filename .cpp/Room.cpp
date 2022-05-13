#include "../.hpp/json.hpp"
#include "../.hpp/Room.h"
#include "../.hpp/XYZ.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace nlohmann;

Room::Room(string title)
: name(title)
{

}

string Room::getName() {
    return name;
}

vector<string> Room::items(XYZ xyz) {
    vector<string> keys;
    json items = xyz.listRooms()[name]["items"];

    for (auto i: items.items()) {
        keys.push_back(i.key());
    }
    return keys;
}