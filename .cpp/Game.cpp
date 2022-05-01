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
      enumChoice(new int(-1)),
      strChoice(new string("")),
      inputType(new string("")),
      exit(new bool(false))
{
    cout << "LOADING: [puppet] ... \n";
    cout << "----------------------------\n";
    
    ifstream i("../.json/options.json");
    json options;
    i >> options;

    if (options["Input Type"] == "Enumeration") {
        *inputType = "Enumeration";
    } else if (options["Input Type"] == "String") {
        *inputType = "String";
    }

    i.close();
}

void Game::getInput() {
    while (!*exit) {
        cin >> input;
        if (*inputType == "Enumeration") { 
            try {
                *enumChoice = stoi(input); // will truncate floats.
                if (*enumChoice < 0)
                    throw 1;
            } catch (int e) {
                cout << "Did not understand input! "
                    << "Please put in a non-negative integer (e.g. 1)."
                    << "\n\nEnter: ";
            }
        }
    }
}

int printOptions(json options) {
    int count = 0;
    for (auto i : options.items()) {
        cout << count << ". " << i.key() << ": " << i.value() << "\n";
        count++;
    }

    const int last = ++count;
    cout << last << ". Back\nEnter: ";

    return last;
}

void Game::options() {
    cout << "\nOptions.\n\n";
    ifstream i("../.json/options.json");
    json options;
    i >> options;

    int last = printOptions(options);

    *enumChoice = -1;
    while (true) {
        if (*inputType == "Enumeration" && *enumChoice > -1) {
            if (*enumChoice == last) break; // Back
            switch(*enumChoice) {
                case 0: { // inputType
                    *enumChoice = -1;
                    cout << "\n0. Enumeration\n1. String\n" <<
                    "2. Cancel\nEnter: ";
                    bool flag = true;
                    while (flag) {
                        if (*enumChoice > -1) {
                            switch(*enumChoice) {
                                case 0: // Enumerator
                                    *inputType = "Enumeration";
                                    options["Input Type"] = "Enumeration";
                                    flag = false;
                                    break;
                                case 1: // String
                                    *inputType = "String";
                                    options["Input Type"] = "String";
                                    flag = false;
                                    break;
                                case 2: // Cancel
                                    flag = false;
                                    break;
                                default:
                                    cout << "Not a valid choice! " <<
                                    "Please pick either 0 or 1.\n";
                            }
                            *enumChoice = -1;
                        }
                    }
                    cout << "\n";
                    printOptions(options);
                    break;
                }
                default:
                    cout << "Not a valid choice!" <<
                    "Please pick between 0 and " <<
                    last << ".\n";
            }
        } else if (*inputType == "String" && *strChoice != "") {
            // By using *strChoice != "", inputs like "0" are allowed.
        }
    }

    i.close();
    ofstream o("../.json/options.json");
    o << options;

    return;
}

void Game::menu() {
    string menuStr = "\n0. New\n1. Resume\n2. Options\n3. Exit\nEnter: ";
    cout << menuStr;

    *enumChoice = -1;
    while (true) {
        if (*inputType == "Enumeration" && *enumChoice > -1) {
            switch (*enumChoice) {
            case 0:
                resume = 0;
                return;
            case 1:
                if (currentFile) {
                    resume = 1;
                    return;
                } else {
                    cout << "\nNo game save!\n";
                    cout << menuStr;
                    break;
                }
            case 2:
                Game::options();
                cout << menuStr;
                break;
            case 3:
                *exit = true;
                return;
            default:
                cout << "Input not in range (e.g. 0-3)!\n\n";
                cout << menuStr;
                break;
            }
            *enumChoice = -1;
        }
    }

    cout << "stop\n";
}

void Game::moving(XYZ xyz, Character zero) {
    vector<string> moves = zero.possibleMoves(xyz);
    zero.printMoves(xyz, moves);

    *enumChoice = -1;
    while (true) {
        if (*inputType == "Enumeration" && *enumChoice > -1) {
            if (*enumChoice < moves.size()) {
                int t = zero.move(xyz, moves[*enumChoice]);
                *time += t;
                *zero.getTimer() += t;
                *enumChoice = -1;
                return;
            } else {
                cout << "Not a valid move!\n\n";
                zero.printMoves(xyz, moves);
            }
            *enumChoice = -1;
        }
    }
}

void Game::toolsMenu(json catalog) {

}

void Game::weaponsMenu(json catalog) {

}

void Game::investing(Character zero) {
    cout << "\n\nInvest:\n" << "Timer: " << *zero.getTimer() << "\n\n";
    ifstream i("../.json/invest.json");
    json catalog;
    i >> catalog;

    int count = 0;
    for (auto i: catalog.items()) {
        cout << count << ". " << i.key() << "\n";
        count++;
    }

    cout << ++count << ". Back\nEnter: ";
    
    // Fix this selection menu (might want to look into recursion).
    *enumChoice = -1;
    while (true) {
        if (*inputType == "Enumeration" && *enumChoice > -1) {
            switch(*enumChoice) {
                case 0: //Tools
                    Game::toolsMenu(catalog);
                    break;
                case 1: //Weapons
                    Game::weaponsMenu(catalog);
                    break;
                case 2: //Back
                    return;
                default: 
                    cout << "Not a valid move!\n\n";
                    for (auto i: catalog.items()) {
                        cout << count << ". " << i.key() << "\n";
                    }
            }
            *enumChoice = -1;
        }
    }

    i.close();
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

    *enumChoice = -1;
    while (true) {
        if (!actions.size()) {
            // No possible movements.
        } else if (*inputType == "Enumeration" && *enumChoice > -1) {
            if (*enumChoice < actions.size()) {
                int univ_choice = univ_actions[actions[*enumChoice]];
                *enumChoice = -1;
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
                    i.close();
                    return;
                default:
                    cout << "Could not find the universal move!\n";
                }
                zero.printActions(xyz, actions);
            } else {
                *enumChoice = -1;
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
