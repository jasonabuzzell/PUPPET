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

// HELPER FUNCTIONS
// -------------------------------------------------------

string decap(string s) {
    for (int i = 0; i < s.length(); i++) {
        s[i] = tolower(s[i]);
    }
    return s;
}

int enumConvert(string s, vector<string> vec) {
    auto itr = find(vec.begin(), vec.end(), s);
    if (itr == vec.end()) {
        return -2;
    } else {
        return distance(vec.begin(), itr);
    }
}

int printJson(json options) {
    int count = 0;
    for (auto i : options.items()) {
        cout << count << ". " << i.key();
        if (i.value().is_number()) cout << ": " << i.value();
        cout << "\n";
        count++;
    }
    cout << count << ". Back\nEnter: ";
    return count;
}

// GAME CLASS
// -----------------------------------------------------

Game::Game()
    : currentFile(0),
      time(new int(0)),
      enumChoice(new int(-1)),
      strChoice(new string("")),
      inputType(new string("")),
      exit(new bool(false)) 
    {
    cout << "LOADING: [puppet] ... \n";
    "----------------------------\n\n";

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

void Game::convertInput(json allChoices) {
    if (*strChoice != "") {
        *enumChoice = enumConvert(*strChoice, allChoices);
        *strChoice = "";
    } 
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
                if (input == "")
                    throw 1;
                for (auto c : input) {
                    if (!isdigit(c))
                        throw 1;
                }
                *enumChoice = stoi(input); // will truncate floats.
            } catch (int e) {
                *enumChoice = -1;
                cout << "Did not understand input! "
                        "Please put in a non-negative integer (e.g. 1)."
                        "\n\nEnter: ";
            }
        } else if (*inputType == "String") {
            *strChoice = decap(input);
        }
    }
}

void updateOptions(json options) {
    ofstream ofs(".json/options.json");
    ofs << options;
    ofs.close();
}

json Game::optionsInputType(json options) {
    vector<string> allInputs = { "enumeration", "string", "cancel" };

    cout << "\nINPUT TYPE:\n\n0. Enumeration\n1. String\n"
            "2. Cancel\nEnter: ";

    Game::choiceReset();

    while (true) {
        Game::convertInput(allInputs);
        if (*enumChoice != -1) {
            switch (*enumChoice) {
            case 0: // Enumerator
                *enumChoice = -1;
                *inputType = "Enumeration";
                options["Input Type"] = "Enumeration";
                return options;
            case 1: // String
                *enumChoice = -1;
                *inputType = "String";
                options["Input Type"] = "String";
                return options;
            case 2: // Cancel
                *enumChoice = -1;
                return options;
            default:
                *enumChoice = -1;
                cout << "Not a valid choice!\n\nEnter: ";
            }
        }
    }
}

void Game::options() {
    vector<string> allOptions = { "input type", "back" };

    cout << "\nOPTIONS:\n\n";
    ifstream ifs(".json/options.json");
    json options = json::parse(ifs);
    ifs.close();
    printJson(options);

    Game::choiceReset();

    while (true) {
        // ENUMERATION OPTIONS
        Game::convertInput(allOptions);
        if (*enumChoice != -1) {
            switch (*enumChoice) {
                case 0: // inputType
                    options = Game::optionsInputType(options);
                    cout << "\n";
                    printJson(options);
                    break;
                case 1: // Back
                    *enumChoice = -1;
                    updateOptions(options);
                    return;
                default:
                    *enumChoice = -1;
                    cout << "Not a valid choice!\n\nEnter: ";
            }
        }
    }
}

void Game::manual() {

}

void Game::moving(XYZ xyz, Character zero) {
    vector<string> moves = zero.possibleMoves(xyz);
    zero.printMoves(xyz, moves);

    Game::choiceReset();

    // ENUMERATION MOVING
    while (true) {
        if (*enumChoice != -1) {
            if (*enumChoice < moves.size()) {
                int t = zero.move(xyz, moves[*enumChoice]);
                *time += t;
                *zero.getTimer() += t;
                *enumChoice = -1;
                return;
            } else {
                *enumChoice = -1;
                cout << "Not a valid move!\n\n";
                zero.printMoves(xyz, moves);
            }
        }
    }
}

void Game::purchasing(Character zero, string key, int value) {
    vector<string> allChoices = { "yes", "no", "cancel" };
    int timer = *zero.getTimer();

    cout << "Purchase " << key << " for " << value << "?\n\n"
            "0. Yes\n1. No\nEnter: "; 
    Game::choiceReset();
    // Add in loan option.
    while (true) {
        Game::convertInput(allChoices);
        if (*enumChoice != -1) {
            switch (*enumChoice) {
                case 0: // Yes
                    if (timer >= value) { 
                        cout << "Purchasing...\n";
                        timer -= value;
                        // Add in item drop.
                    } else {
                        cout << "Insufficient funds!\n\n";
                    }
                    *enumChoice = -1;
                    break;
                case 1: // No
                    *enumChoice = -1;
                    return;
            }
            cout << "Purchase " << key << " for " << value << "?\n\n"
                    "0. Yes\n1. No\nEnter: "; 
        }
    }
}

void Game::investingInner(Character zero, json catalog) {
    vector<string> allItems;
    vector<string> capItems;

    cout << "\nTimer: " << *zero.getTimer() << "\n\n";
    for (auto i: catalog.items()) {
        // Won't need to check for correct type due to logic.
        allItems.push_back(decap(i.key()));
        capItems.push_back(i.key()); // forced to due this to access.
    }
    int count = printJson(catalog); // O(2i): Double for-loop.
    Game::choiceReset();
    while (true) {
        Game::convertInput(allItems);
        if (*enumChoice != -1) {
            if (*enumChoice > -1 && *enumChoice < allItems.size()) {
                string key = capItems[*enumChoice];
                auto value = catalog[key];
                *enumChoice = -1;
                if (value.is_number()) {
                    Game::purchasing(zero, key, value);
                } else {
                    Game::investingInner(zero, value);
                }
            } else if (*enumChoice == count) { // Back
                *enumChoice = -1;
                return;                
            } else {
                *enumChoice = -1;
                cout << "Not a valid choice!\n\n";
            }
            cout << "\nTimer: " << *zero.getTimer() << "\n\n";
            count = printJson(catalog);
        }
    }
}

void Game::investing(Character zero) {
    vector<string> allActions = { "tools", "weapons", "back" };

    cout << "\n\nINVEST:";
    
    ifstream ifs(".json/invest.json");
    json catalog = json::parse(ifs);
    ifs.close();

    // Fix this selection menu (might want to look into recursion).
    Game::investingInner(zero, catalog);
}

void Game::play() {
    XYZ xyz;
    User zero("Start Zero");
    int univChoice = -1;
    vector<string> allActions = { "exit", "move", "look", "use",
                                "invest", "automate", "wait", 
                                "manual", "options", "save" };
    vector<int>::iterator itr;
    string info = "\nTime: " + to_string(*time) + "\nLocation: " 
                    + zero.getLocation() + "\n\n";

    cout << info;
    vector<string> actions = zero.possibleActions();
    zero.printActions(xyz, actions);

    Game::choiceReset();

    while (true) {
        if (!actions.size()) {
            // No possible movements.
        }
        Game::convertInput(allActions);
        if (*enumChoice != -1) {
            switch (*enumChoice) {
                case 0: // Move
                    *enumChoice = -1;
                    Game::moving(xyz, zero);
                    break;
                case 1: // Look
                    *enumChoice = -1;
                    zero.look(xyz);
                    break;
                case 2: // Use
                    *enumChoice = -1;
                    break;
                case 3: // Invest
                    *enumChoice = -1;
                    Game::investing(zero);
                    break;
                case 4: // Automate
                    *enumChoice = -1;
                    break;
                case 5: // Wait
                    *enumChoice = -1;
                    break;
                case 6: // Manual;
                    *enumChoice = -1;
                    Game::manual();
                    break;
                case 7: // Options
                    *enumChoice = -1;
                    Game::options();
                    break;
                case 8: // Save
                    *enumChoice = -1;
                    break;
                case 9: // Exit (to menu/save or desktop)
                    *enumChoice = -1;
                    *exit = true;
                    return;
                default:
                    *enumChoice = -1;
                    cout << "Not a valid choice!\n";
            }

        cout << info;
        zero.printActions(xyz, actions);
        }
    }
}

void Game::menu() {
    vector<string> allChoices = { "new", "resume", "options", 
                                "manual", "exit" };
    string menuStr = "\n0. New\n1. Resume\n2. Options\n3. Manual"
                     "\n4. Exit\nEnter: ";
    cout << menuStr;

    Game::choiceReset();

    while (true) {
        Game::convertInput(allChoices);
        if (*enumChoice != -1) {
            switch (*enumChoice) {
            case 0: // New
                *enumChoice = -1;
                resume = 0;
                return;
            case 1: // Resume
                if (currentFile) {
                    *enumChoice = -1;
                    resume = 1;
                    return;
                } else {
                    *enumChoice = -1;
                    cout << "\nNo game save!\n" << menuStr;
                    break;
                }
            case 2: // Options
                *enumChoice = -1;
                Game::options();
                cout << menuStr;
                break;
            case 3: // Manual
                *enumChoice = -1;
                Game::manual();
                break;
            case 4: // Exit
                *exit = true;
                *enumChoice = -1;
                return;
            default:
                *enumChoice = -1;
                cout << "Input not in range (e.g. 0-3)!\n"
                     << menuStr;
                break;
            }
        }
    }
}

void Game::main() {
    Game::menu();
    if (*exit)
        return;

    // Make enemies.
    // Inject personality into each character (1 - Playful)
    // for both Artivox and game preferences.
    // Enemy AI.
    // Make save state.
    // Figure out investing screen.
    // Figure out incentives (e.g. exploring a room).
    Game::play();

    return;
}
