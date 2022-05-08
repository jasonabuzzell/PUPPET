#include "../.hpp/Character.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/json.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;
using namespace nlohmann;

// CHARACTER
// ---------------------------------------------------------

Character::Character(XYZ xyz, string place, string part)
    : location(new string(place)),
      point(new string(part)),
      x(new float(0.0)),
      y(new float(0.0)),
      z(new float(0.0)),
      timer(new int(0)),
      moveFlag(true),
      lookFlag(true),
      ableFlag(true) 
{
    vector<int> coords = xyz.listRooms()[*location][*point]["coords"];
    *x = coords[0];
    *y = coords[1];
    *z = coords[2];
}

void Character::setLocation(string loc) {
    *location = loc;
}

void Character::setPoint(string poi) {
    *point = poi;
}

string *Character::getLocation() {
    return location;
}

string *Character::getPoint() {
    return point;
}

int *Character::getTimer() {
    return timer;
}

vector<string> Character::possibleActions() {
    vector<string> actions;

    if (moveFlag)
        actions.push_back("Move");
    if (ableFlag)
        actions.push_back("Use");
    actions.push_back("Invest");
    actions.push_back("Wait");

    return actions;
}

void Character::printActions(XYZ xyz, vector<string> actions) {
    for (int i = 0; i < actions.size(); i++) {
        cout << i << ". " << actions[i] << "\n";
    }
    cout << "Enter: ";
}

json Character::possibleMoves(XYZ xyz) {
    json moves;
    json rooms = xyz.listRooms();
    vector<int> a = rooms[*location][*point]["coords"];
    json connected = rooms[*location][*point]["connect"];
    for (auto room: connected.items()) {
        moves[room.key()] = {};
        for (auto point: room.value()) { 
            vector<int> b = rooms[room.key()][point]["coords"];
            moves[room.key()][point] = {};
            // Uses sqrt() twice!
            int hypo = round(sqrt(pow(b[0] - a[0], 2) + pow(b[1] - a[1], 2)));
            int dist = round(sqrt(hypo + pow(b[2]- a[2], 2))); 
            moves[room.key()][point]["Walking"] = dist; // like 1m/s.
            moves[room.key()][point]["Crouching"] = dist * 3;
            moves[room.key()][point]["Running"] = static_cast<int>(round(float(dist) / 3));
        }
    }
    return moves;
}

void Character::printMoves(XYZ xyz, vector<string> moves) {
    cout << "\nRooms:\n";
    for (int i = 0; i < moves.size(); i++) {
        cout << i << ". " << moves[i] << "\n";
    }
    cout << "Enter: ";
}

void Character::move(string room, string part, vector<int> coords, 
                    int expTime, int *time) 
{
    float startX = *x;
    float startY = *y;
    float startZ = *z;
    for (int i = 0; i < expTime; i++) {
        cout << "Time: " << (*time)++; 
        *x = (coords[0] - startX) * (i / float(expTime)) + startX;
        *y = (coords[1] - startY) * (i / float(expTime)) + startY;
        *z = (coords[2] - startZ) * (i / float(expTime)) + startZ;
        cout << " X: " << round(*x) << " Y: " 
             << round(*y) << " Z: " << round(*z) << "\n";
        Sleep(1000);
    }
    *x = coords[0];
    *y = coords[1];
    *z = coords[2];
    *location = room;
    *point = part;
    return;
}

// USER
// ----------------------------------------------------------

vector<string> User::possibleActions() {
    vector<string> actions;

    if (moveFlag) actions.push_back("Move");
    if (lookFlag) actions.push_back("Look");
    if (ableFlag) actions.push_back("Use");
    actions.push_back("Invest");
    actions.push_back("Automate");
    actions.push_back("Wait");
    actions.push_back("Manual");
    actions.push_back("Options");
    actions.push_back("Save");
    actions.push_back("Exit");

    return actions;
}

void User::look(XYZ xyz) {
    vector<string> connect = xyz.listRooms()[*location]["connect"];
    cout << "\nLooking...\nRooms:\n";
    for (auto s : connect) {
        cout << s << "\n";
    }
}