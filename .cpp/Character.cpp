#include "../.hpp/Character.h"
#include "../.hpp/Helper.h"
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

Character::Character(XYZ xyz, string title, string place, string part, bool use)
    : location(new string(place)),
      point(new string(part)),
      name(title),
      user(use),
      x(new float(0.0)),
      y(new float(0.0)),
      z(new float(0.0)),
      moveFlag(true),
      lookFlag(true),
      ableFlag(true) 
{
    vector<int> coords = xyz.listRooms()[*location][*point]["Coords"];
    *x = coords[0];
    *y = coords[1];
    *z = coords[2];
}

string Character::getName() {
    return name;
}

bool Character::getUser() {
    return user;
}

void Character::setParameters(vector<string> position, vector<float> coords, vector<bool> flags) {
    *location = position[0];
    *point = position[1];
    *x = coords[0];
    *y = coords[1];
    *z = coords[2];
    moveFlag = flags[0];
    ableFlag = flags[1];
    lookFlag = flags[2];
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
    return {*x, *y, *z};
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

int Character::distance(vector<float> a, vector<float> b) {
    int hypo = round(sqrt(pow(b[0] - a[0], 2) + pow(b[1] - a[1], 2)));
    int dist = round(sqrt(hypo + pow(b[2] - a[2], 2)));
    return dist;
}

json Character::distanceTime(vector<float> a, vector<float> b) {
    json movement;
    // Very demanding equation, might want to consider alternatives.
    int dist = Character::distance(a, b);
    movement["Walking"] = dist; // like 1m/s.
    movement["Crouching"] = dist * 3;
    movement["Running"] = int(ceil(float(dist) / 3));
    return movement;
}

json Character::possibleMoves(XYZ xyz) {
    json moves;
    json rooms = xyz.listRooms();
    vector<float> a = rooms[*location][*point]["Coords"];
    json connected = rooms[*location][*point]["Connect"];
    for (auto room : connected.items()) {
        moves[room.key()] = {};
        for (auto point : room.value()) {
            vector<float> b = rooms[room.key()][point]["Coords"];
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

void Character::move(vector<float> curCoords, vector<float> expCoords, int curTime, int expTime) {
    vector<float*> coords = {x, y, z};
    float percentage = curTime / float(expTime);
    for (int i = 0; i < coords.size(); i++) {
        *coords[i] = (expCoords[i] - curCoords[i]) * percentage + curCoords[i];
    }
    cout << "X: " << setprecision(2) << *x << ", Y: "
         << *y << ", Z: " << *z << "...\n";
}

json Character::look(XYZ xyz, json actions) {
    json look;
    look = json({});

    look["Characters"] = json({});
    look["Characters"]["Time"] = 2;

    look["Rooms"] = json({});
    look["Rooms"]["Time"] = 4;
    
    look["Room Items"] = json({});
    look["Room Items"]["Time"] = 3;

    look["Inventory Items"] = json({});
    look["Inventory Items"]["Time"] = 2;
    
    actions[name]["Look"] = look;
    actions["Active"] = true;
    
    return actions;
}

json Character::compareCoords(XYZ xyz, json rooms, vector<string> points, string room, string part) {
    vector<float> newCoords;
    string newPoint;  
    int best = -1;          
    for (string point: points) {
        vector<float> coords = rooms[room][point]["Coords"];
        int dist = Character::distance(rooms[room][part]["Coords"], coords);
        if (best > dist || best == -1) {
            best = dist;
            newCoords = coords;
            newPoint = point; 
        }
    }
    json compare;
    compare["Best"] = best;
    compare["New Point"] = newPoint;
    compare["New Coords"] = newCoords;

    return compare;
}