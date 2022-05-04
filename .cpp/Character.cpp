#include "../.hpp/Character.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/json.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace nlohmann;

Character::Character(string place)
    : location(new string(place)),
      timer(new int(0)),
      moveFlag(true),
      lookFlag(true),
      ableFlag(true) {}

string Character::getLocation() {
    return *location;
}

int *Character::getTimer() {
    return timer;
}

vector<string> Character::possibleActions() {
    vector<string> actions;

    if (moveFlag) actions.push_back("Move");
    if (ableFlag) actions.push_back("Use");
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

vector<string> Character::possibleMoves(XYZ xyz) {
    vector<string> rooms = xyz.listRooms()[*location]["connect"];
    rooms.push_back("Cancel");
    return rooms;
}

void Character::printMoves(XYZ xyz, vector<string> moves) {
    cout << "\nRooms:\n";
    for (int i = 0; i < moves.size(); i++) {
        cout << i << ". " << moves[i] << "\n";
    }
    cout << "Enter: ";
}

int Character::move(XYZ xyz, string room) {
    if (room == "Cancel") {
        return 0;
    } else {
        cout << "Moving...\n";
        vector<int> a = xyz.listRooms()[*location]["coordinates"];
        vector<int> b = xyz.listRooms()[room]["coordinates"];
        int c = round((pow(b[0] - a[0], 2) + pow(b[1] - a[1], 2)) / 10);
        *location = room;
        return c;
    }
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
