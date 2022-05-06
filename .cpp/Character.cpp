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

Character::Character(XYZ xyz, string place)
    : location(new string(place)),
      x(new float(0.0)),
      y(new float(0.0)),
      z(new float(0.0)),
      timer(new int(0)),
      moveFlag(true),
      lookFlag(true),
      ableFlag(true) {
    vector<int> coordinates = xyz.listRooms()[*location]["coordinates"];
    *x = coordinates[0];
    *y = coordinates[1];
}

string *Character::getLocation() {
    return location;
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
    vector<int> a = rooms[*location]["coordinates"];
    for (auto i : rooms[*location]["connect"]) {
        vector<int> b = rooms[i]["coordinates"];
        moves[i] = {};
        // Uses sqrt().
        int dist = round(sqrt(pow(b[0] - a[0], 2) + pow(b[1] - a[1], 2)));
        moves[i]["Walking"] = dist; // like 1m/s.
        moves[i]["Crouching"] = dist * 3;
        moves[i]["Running"] = static_cast<int>(round(float(dist) / 3));
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

void Character::move(string room, vector<int> coords, 
                    int expTime, int *time) 
{
    float startX = *x;
    float startY = *y;
    for (int i = 0; i < expTime; i++) {
        cout << "Time: " << (*time)++; 
        *x = (coords[0] - startX) * (i / float(expTime)) + startX;
        *y = (coords[1] - startY) * (i / float(expTime)) + startY;
        cout << " X: " << round(*x) << " Y: " 
             << round(*y) << "\n";
        Sleep(1000);
    }
    *x = coords[0];
    *y = coords[1];
    *location = room;
    return;
}

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
