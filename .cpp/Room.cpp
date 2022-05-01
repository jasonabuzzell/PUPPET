#include "../.hpp/json.hpp"
#include "../.hpp/Room.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace nlohmann;

Room::Room(string title)
: name(title)
{

}