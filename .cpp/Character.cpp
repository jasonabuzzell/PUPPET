#include "../.hpp/Character.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/json.hpp"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace nlohmann;

Character::Character(string place)
    : location(new string(place)),
      moveFlag(true),
      lookFlag(true),
      ableFlag(false) 
{}

string Character::getLocation() {
    return *location;
}

vector<string> Character::possibleActions() {
    vector<string> actions;

    if (moveFlag) actions.push_back("Move");
    if (ableFlag) actions.push_back("Take");

    return actions;
}

void Character::printActions(XYZ xyz, vector<string> actions) {
    for (int i = 0; i < actions.size(); i++) {
        cout << i << ". " << actions[i] << "\n";
    }
    cout << "Enter: ";
}

vector <string> Character::possibleMoves(XYZ xyz) {
    vector <string> rooms = xyz.listRooms()[*location]["connect"];
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

void Character::move(string room) {
    if (room == "Cancel") {
        return;
    } else {
        cout << "Moving...\n";
        *location = room;
    }
}

// Add in functionality for Character with each room using 
// Room class. (e.g. "terminal" gives action to change
// XYZ config.)

vector<string> User::possibleActions() {
    vector<string> actions;

    if (moveFlag) actions.push_back("Move");
    if (lookFlag) actions.push_back("Look");
    if (ableFlag) actions.push_back("Take");
    actions.push_back("Options");
    actions.push_back("Exit");

    return actions;
}

void User::look(XYZ xyz) {
    vector<string> connect = xyz.listRooms()[*location]["connect"];
    cout << "\nLooking...\nRooms:\n";
    for (auto s: connect) {
        cout << s << "\n";
    }
    cout << "\n";
}
