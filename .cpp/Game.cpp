#include "../.hpp/Game.h"
#include "../.hpp/Character.h"
#include "../.hpp/Room.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;
using namespace nlohmann;

Game::Game()
    : currentFile(0),
      choice(new int(-1)),
      exit(new bool(false)) {
    cout << "LOADING: [puppet] ... \n";
    cout << "----------------------------\n";
}

void Game::getInput() {
    while (!*exit) {
        cin >> input;
        try {
            *choice = stoi(input); //will truncate floats.
            if (*choice < 0)
                throw 1;
        } catch (int e) {
            cout << "Did not understand input! "
                 << "Please put in a non-negative integer (e.g. 1)."
                 << "\n\nEnter: ";
        }
    }
}

void Game::options() {
    cout << "\nOptions.\n";
    Sleep(1000); // add options
    return;
}

void Game::menu() {
    string menuStr = "\n0. New\n1. Resume\n2. Options\n3. Exit\nEnter: ";
    cout << menuStr;
    while (true) {
        if (*choice > -1) {
            switch (*choice) {
                case 0:
                    resume = 0;
                    *choice = -1;
                    return;
                case 1:
                    if (currentFile) {
                        resume = 1;
                        *choice = -1;
                        return;
                    } else {
                        cout << "\nNo game save!\n";
                        *choice = -1;
                        cout << menuStr;
                        break;
                    }
                case 2:
                    *choice = -1;
                    Game::options();
                    cout << menuStr;
                    break;
                case 3:
                    *exit = true;
                    *choice = -1;
                    return;
                default:
                    cout << "Input not in range (e.g. 0-3)!\n\n";
                    *choice = -1;
                    cout << menuStr;
                    break;
            }
        }
    }

    cout << "stop\n";
}

void Game::moving(XYZ xyz, User zero) {
    vector<string> moves = zero.possibleMoves(xyz);
    zero.printMoves(xyz, moves);

    while (true) {
        if (*choice > -1) {
            if (*choice < moves.size()) {
                zero.move(moves[*choice]);
                *choice = -1;
                return;
            } else {
                cout << "Not a valid move!\n\n";
                zero.printMoves(xyz, moves);
                *choice = -1;
            }
        }
    }
}

void Game::play() {
    XYZ xyz;
    User zero("start_zero");

    cout << "\nLocation: " << zero.getLocation() << "\n";
    vector<string> actions = zero.possibleActions();
    zero.printActions(xyz, actions);
    ifstream i("../.json/actions.json");
    json univ_actions;
    i >> univ_actions;

    while (true) {
        if (!actions.size()) {
            // No possible movements.
        } else if (*choice > -1) {
            if (*choice < actions.size()) {
                int univ_choice = univ_actions[actions[*choice]];
                *choice = -1;
                switch (univ_choice) {
                    case 0: // Move
                        Game::moving(xyz, zero);
                        cout << "\nLocation: " << zero.getLocation() << "\n";
                        break;
                    case 1: // Look
                        zero.look(xyz);
                        break;
                    case 2: // Use
                        break;
                    case 3: // Take
                        // Make a .json file for weapons/tools with parameters
                        // for starting position and current position, 
                        // (e.g. start = "hall_one", current = "zero_inventory")
                        Game::taking(xyz, zero);
                        break;
                    case 4: // Drop
                        break;
                    case 5: // Options
                        Game::options();
                        break;
                    case 6: // Exit (to menu/save or desktop)
                        *exit = true;
                        return;
                    default:
                        cout << "Could not find the universal move!\n";
                }
                zero.printActions(xyz, actions);
            } else {
                *choice = -1;
                cout << "Not a valid choice! "
                     << "Pick something between 0 and " << actions.size() - 1 << ".\n";
            }
        }
    }
}

void Game::main() {
    Game::menu();
    if (*exit) return;
    Game::play();

    return;
}
