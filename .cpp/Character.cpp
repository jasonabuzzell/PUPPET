#include "../.hpp/Character.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/json.hpp"
#include "../.hpp/Helper.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;
using namespace nlohmann;

// CHARACTER
// ---------------------------------------------------------

Character::Character(XYZ xyz, string title, string place, string part)
    : location(new string(place)),
      point(new string(part)),
      name(title),
      x(new float(0.0)),
      y(new float(0.0)),
      z(new float(0.0)),
      moveFlag(true),
      lookFlag(true),
      ableFlag(true) 
{
    vector<int> coords = xyz.listRooms()[*location][*point]["coords"];
    *x = coords[0];
    *y = coords[1];
    *z = coords[2];
}

string Character::getName() {
    return name;
}

void Character::setLocation(string loc) {
    *location = loc;
}

void Character::setPoint(string poi) {
    *point = poi;
}

void Character::setCoords(vector<float> coords) {
    *x = coords[0];
    *y = coords[1];
    *z = coords[2];
}

string *Character::getLocation() {
    return location;
}

string *Character::getPoint() {
    return point;
}

float *Character::getX() {
    return x;
}

float *Character::getY() {
    return y;
}

float *Character::getZ() {
    return z;
}

vector<float> Character::getCoords() {
    return { *x, *y, *z };
}

bool Character::getMoveFlag() {
    return moveFlag;
}

bool Character::getLookFlag() {
    return lookFlag;
}

bool Character::getAbleFlag() {
    return ableFlag;
}

void Character::printActions(XYZ xyz, vector<string> actions) {
    for (int i = 0; i < actions.size(); i++) {
        cout << i << ". " << actions[i] << "\n";
    }
    cout << "Enter: ";
}

json Character::distanceTime(vector<float> a, vector<float> b) {
    json movement;
    // Very demanding equation, might want to consider alternatives.
    int hypo = round(sqrt(pow(b[0] - a[0], 2) + pow(b[1] - a[1], 2)));
    int dist = round(sqrt(hypo + pow(b[2]- a[2], 2))); 
    movement["Walking"] = dist; // like 1m/s.
    movement["Crouching"] = dist * 3;
    movement["Running"] = static_cast<int>(round(float(dist) / 3));
    return movement;
}

json Character::possibleMoves(XYZ xyz) {
    json moves;
    json rooms = xyz.listRooms();
    vector<float> a = rooms[*location][*point]["coords"];
    json connected = rooms[*location][*point]["connect"];
    for (auto room: connected.items()) {
        moves[room.key()] = {};
        for (auto point: room.value()) { 
            vector<float> b = rooms[room.key()][point]["coords"];
            moves[room.key()][point] = {};
            json movement = Character::distanceTime(a, b);
            moves[room.key()][point]["Walking"] = movement["Walking"];
            moves[room.key()][point]["Crouching"] = movement["Crouching"];
            moves[room.key()][point]["Running"] = movement["Running"];
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

void Character::move(vector<float> curCoords, vector<float> expCoords, int curTime, int expTime) 
{
    vector<float*> coords = { x, y, z };
    float percentage = curTime / float(expTime);
    for (int i = 0; i < coords.size(); i++) {
        *coords[i] = (expCoords[i] - curCoords[i]) * percentage + curCoords[i];
    }
    cout << " X: " << setprecision(2) << *x << " Y: " 
            << *y << " Z: " << *z << "\n";
}

json Character::look(XYZ xyz, json actions) {
    actions[name]["Look"] = json({});

    actions[name]["Look"]["Rooms"] = json({});
    actions[name]["Look"]["Rooms"]["Time"] = 4;
    json room = xyz.listRooms()[*location];
    json connect = room[*point]["connect"];
    for (auto room: connect.items()) {
        actions[name]["Look"]["Rooms"][room.key()] = {};
        for (auto part: room.value()) {
            actions[name]["Look"]["Rooms"][room.key()].push_back(part);
        }
    }

    actions[name]["Look"]["Room Items"] = json({});
    actions[name]["Look"]["Room Items"]["Time"] = 3;
    actions[name]["Look"]["Room Items"]["Items"] = xyz.getItems(*location);
    
    actions[name]["Look"]["Inventory Items"] = json({});
    actions[name]["Look"]["Inventory Items"]["Time"] = 2;
    actions[name]["Look"]["Inventory Items"]["Items"] = xyz.getItems(name);

    actions["Active"] = true;
    return actions;
}