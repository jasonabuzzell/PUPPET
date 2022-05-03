#include "../.hpp/json.hpp"
#include "../.hpp/Game.h"
#include "../.hpp/Character.h"
#include "../.hpp/Room.h"
#include "../.hpp/XYZ.h"
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;
using namespace nlohmann;

string decap(string s) {
    for (int i = 0; i < s.length(); i++) {
        s[i] = tolower(s[i]);
    }
    return s; 
} 

Game::Game()
    : currentFile(0),
      time(new int(0)),
      enumChoice(new int(-1)),
      strChoice(new string("")),
      inputType(new string("")),
      exit(new bool(false))
{
    cout << "LOADING: [puppet] ... \n";
    cout << "----------------------------\n\n";
    
    ifstream ifs(".json/options.json");
    json options = json::parse(ifs);
    ifs.close();

    if (options["Input Type"] == "Enumeration") {
        *inputType = "Enumeration";
    } else if (options["Input Type"] == "String") {
        *inputType = "String";
    }

    cout << "Input Type: " << *inputType << "\n";
}

void Game::choiceReset() {
    *enumChoice = -1;
    *strChoice = "";
}

void Game::getInput() {
    while (!*exit) {
        getline(cin, input);
        if (*inputType == "Enumeration") { 
            try {
                if (input == "") throw 1;
                *enumChoice = stoi(input); // will truncate floats.
                if (*enumChoice < 0)
                    throw 1;
            } catch (int e) {
                cout << "Did not understand input! "
                    << "Please put in a non-negative integer (e.g. 1)."
                    << "\n\nEnter: ";
            }
        } else if (*inputType == "String") {
            *strChoice = decap(input);
        }
    }
}

void printOptions(json options) {
    int count = 0;
    for (auto i : options.items()) {
        cout << count << ". " << i.key() << ": " << i.value() << "\n";
        count++;
    }
    cout << count << ". Back\nEnter: ";
}

void updateOptions(json options) {
    ofstream ofs(".json/options.json");
    ofs << options;
    ofs.close();
}

void Game::options() {
    cout << "\nOptions.\n\n";
    ifstream ifs(".json/options.json");
    json options = json::parse(ifs);
    ifs.close();
    printOptions(options);

    Game::choiceReset();
    while (true) {
        if (*enumChoice > -1 && *inputType == "Enumeration") {
            switch(*enumChoice) {
                case 0: {// inputType
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
                                    "Please pick either 0 or 1.\n\nEnter: ";
                            }
                            *enumChoice = -1;
                        }
                    }
                    cout << "\n";
                    printOptions(options);
                    break;
                }
                case 1: // Back
                    *enumChoice = -1;
                    updateOptions(options);
                    return;
                default:
                    cout << "Not a valid choice!" <<
                    "Please pick between 0 and 1.\n\nEnter: ";
            }
        } else if (*strChoice != "" && *inputType == "String") {
            if (*strChoice == "input type") {
                *strChoice = "";
                cout << "\n0. Enumeration\n1. String\n" <<
                "2. Cancel\nEnter: ";
                bool flag = true;
                while (flag) {
                    if (*strChoice != "") {
                        if (*strChoice == "enumeration") {
                            *inputType = "Enumeration";
                            options["Input Type"] = "Enumeration";
                            flag = false;
                        } else if (*strChoice == "string") {
                            *inputType = "String";
                            options["Input Type"] = "String";
                            flag = false;
                        } else if (*strChoice == "cancel") {
                            flag = false;
                        } else {
                            cout << "Not a valid choice! " <<
                            "Please pick either 0 or 1.\n\nEnter: ";
                        }
                        *strChoice = "";
                    }
                }
                cout << "\n";
                printOptions(options);
            } else if (*strChoice == "back") {
                *strChoice = "";
                updateOptions(options);
                return;
            } else {
                cout << "Not a valid choice! " <<
                "Please pick an option (e.g. Enumeration)" << 
                "\n\nEnter: ";
                *strChoice = "";                                 
            }
        }
    }
}

void Game::menu() {
    string menuStr = "\n0. New\n1. Resume\n2. Options\n3. Exit\nEnter: ";
    cout << menuStr;

    Game::choiceReset();
    while (true) {
        if (*enumChoice > -1 && *inputType == "Enumeration") {
            switch (*enumChoice) {
            case 0: // New
                resume = 0;
                return;
            case 1: // Resume
                if (currentFile) {
                    resume = 1;
                    return;
                } else {
                    cout << "\nNo game save!\n";
                    cout << menuStr;
                    break;
                }
            case 2: // Options
                Game::options();
                cout << menuStr;
                break;
            case 3: // Exit
                *exit = true;
                return;
            default:
                cout << "Input not in range (e.g. 0-3)!\n";
                cout << menuStr;
                break;
            }
            *enumChoice = -1;
        } else if (*strChoice != "" && *inputType == "String") {
            if (*strChoice == "new") {
                resume = 0;
                return;
            } else if (*strChoice == "resume") {
                if (currentFile) {
                    resume = 1;
                    return;
                } else {
                    cout << "\nNo game save!\n";
                    cout << menuStr;
                    break;
                }
            } else if (*strChoice == "options") {
                Game::options();
                cout << menuStr;
            } else if (*strChoice == "exit") {
                *exit = true;
                return;
            } else {
                cout << "Not a valid choice! " <<
                "Please pick an option (e.g. Enumeration)\n\n";
                cout << menuStr;
            }
            *strChoice = "";
        }
    }

    cout << "stop\n";
}

void Game::moving(XYZ xyz, Character zero) {
    vector<string> moves = zero.possibleMoves(xyz);
    zero.printMoves(xyz, moves);

    Game::choiceReset();
    while (true) {
        if (*enumChoice > -1 && *inputType == "Enumeration") {
            cout << "0\n";
            if (*enumChoice < moves.size()) {
                cout << "1\n";
                int t = zero.move(xyz, moves[*enumChoice]);
                cout << "2\n";
                *time += t;
                *zero.getTimer() += t;
                *enumChoice = -1;
                return;
            } else {
                cout << "Not a valid move!\n\n";
                zero.printMoves(xyz, moves);
            }
            *enumChoice = -1;
        } else if (*strChoice != "" && *inputType == "String") {
            vector<string> decapMoves = {};
            for (auto s: moves) {
                decapMoves.push_back((s));
            }
            if (find(decapMoves.begin(), decapMoves.end(), *strChoice) 
            != decapMoves.end()) {
                int t = zero.move(xyz, *strChoice);
                *time += t;
                *zero.getTimer() += t;
                *strChoice = -1;
                return;
            } else {
                cout << "Not a valid move!\n\n";
                zero.printMoves(xyz, moves);
            }
            *strChoice = -1;
        }
    }
}

void Game::toolsMenu(json catalog) {

}

void Game::weaponsMenu(json catalog) {

}

void Game::investing(Character zero) {
    cout << "\n\nInvest:\n" << "Timer: " << *zero.getTimer() << "\n\n";
    ifstream ifs(".json/invest.json");
    json catalog = json::parse(ifs);
    ifs.close();

    int count = 0;
    for (auto i: catalog.items()) {
        cout << count << ". " << i.key() << "\n";
        count++;
    }

    cout << ++count << ". Back\nEnter: ";
    
    // Fix this selection menu (might want to look into recursion).
    Game::choiceReset();
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
                    count = 0;
                    for (auto i: catalog.items()) {
                        cout << count << ". " << i.key() << "\n";
                    }
            }
            *enumChoice = -1;
        }
    }

    return;
}

void Game::play() {
    XYZ xyz;
    User zero("Start Zero");

    cout << "\nTime: " << *time;
    cout << "\nLocation: " << zero.getLocation() << "\n\n";
    vector<string> actions = zero.possibleActions();
    zero.printActions(xyz, actions);
    ifstream ifs(".json/actions.json");
    json univ_actions = json::parse(ifs);
    ifs.close();

    Game::choiceReset();
    while (true) {
        if (!actions.size()) {
            // No possible movements.
        } else if (*enumChoice > -1 && *inputType == "Enumeration") {
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
                case 6: // Automate
                    cout << "\n";
                    break;
                case 7: // Wait
                    cout << "\n";
                    break;
                case 8: // Options
                    Game::options();
                    break;
                case 9: // Save
                    cout << "\n";
                    break;
                case 10: // Exit (to menu/save or desktop)
                    *exit = true;
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
        } else if (*strChoice != "" && *inputType == "String") {
            if (*strChoice == "move") {
                Game::moving(xyz, zero);
                cout << "\nTime: " << *time;
                cout << "\nLocation: " << zero.getLocation() << "\n\n";
            } else if (*strChoice == "look") {
                zero.look(xyz);
            } else if (*strChoice == "use") {
                cout << "\n";
            } else if (*strChoice == "take") {
                // Make a .json file for weapons/tools with parameters
                // for starting position and current position,
                // (e.g. start = "hall_one", current = "zero_inventory")
                cout << "\n";
            } else if (*strChoice == "drop") {
                cout << "\n"; // Don't remove these, they will become part of the function
            } else if (*strChoice == "invest") {
                Game::investing(zero);
                cout << "\n";
            } else if (*strChoice == "automate") {
                cout << "\n";
            } else if (*strChoice == "wait") {
                cout << "\n";
            } else if (*strChoice == "options") {
                Game::options();
            } else if (*strChoice == "save") {
                cout << "\n";
            } else if (*strChoice == "exit") {
                *exit = true;
                return;
            } else {
                cout << "Could not find the universal move!\n";
            }
        }
    }
}

void Game::main() {
    Game::menu();
    if (*exit)
        return;
    
    // Fix nlohmann issues.
    // Make enemies.
    // Make save state.
    // Figure out investing screen.
    // Figure out incentives (e.g. exploring a room).
    Game::play();

    return;
}
