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
      time(new int(0)),
      choice(new int(-1)),
      exit(new bool(false)) {
    cout << "LOADING: [puppet] ... \n";
    cout << "----------------------------\n";
}

void Game::getInput() {
    while (!*exit) {
        cin >> input;
        try {
            *choice = stoi(input); // will truncate floats.
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
    cout << "\nOptions.\n\n";
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

void Game::moving(XYZ xyz, Character zero) {
    vector<string> moves = zero.possibleMoves(xyz);
    zero.printMoves(xyz, moves);

    while (true) {
        if (*choice > -1) {
            if (*choice < moves.size()) {
                int t = zero.move(xyz, moves[*choice]);
                *time += t;
                *zero.getTimer() += t;
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

void Game::investing(Character zero) {
    cout << "\nInvest:\n" << "Timer: " << *zero.getTimer() << "\n\n";
    ifstream i("../.json/invest.json");
    json catalog;
    i >> catalog;

    int count = 0;
    for (auto i: catalog.items()) {
        cout << count << ". " << i.key() << "\n";
    }
    
    // Fix this selection menu (might want to look into recursion).
    while (true) {
        if (*choice > -1) {
            switch(*choice) {
                case 0: //Tools
                    *choice = -1;
                    break;
                case 1: //Weapons
                    *choice = -1;
                    break;
                default: 
                    cout << "Not a valid move!\n\n";
                    for (auto i: catalog.items()) {
                        cout << count << ". " << i.key() << "\n";
                    }
                    *choice = -1;
            }
        }
    }

    return;
}

void Game::play() {
    XYZ xyz;
    User zero("start_zero");

    cout << "\nTime: " << *time;
    cout << "\nLocation: " << zero.getLocation() << "\n\n";
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
                    cout << "\nTime: " << *time;
                    cout << "\nLocation: " << zero.getLocation() << "\n\n";
                    break;
                case 1: // Look
                    zero.look(xyz);
                    break;
                case 2: // Use
                    cout << "\n";
                    break;
                case 3: // Take
                    // Make a .json file for weapons/tools with parameters
                    // for starting position and current position,
                    // (e.g. start = "hall_one", current = "zero_inventory")
                    cout << "\n";
                    break;
                case 4: // Drop
                    cout << "\n";
                    break;
                case 5: // Invest
                    Game::investing(zero);
                    cout << "\n";
                    break;
                case 6: //Automate
                    cout << "\n";
                    break;
                case 7: // Options
                    Game::options();
                    break;
                case 8: // Save
                    cout << "\n";
                    break;
                case 9: // Exit (to menu/save or desktop)
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
    if (*exit)
        return;
    Game::play();

    return;
}
