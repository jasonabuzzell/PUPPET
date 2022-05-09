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

string cap(string s) {
    s[0] = toupper(s[0]);
    for (int i = 1; i < s.length(); i++) {
        if (s[i-1] == ' ') {
            s[i] = toupper(s[i]);
        }
    }
    return s;
}

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

int printJson(json dict) {
    int count = 0;
    cout << "\n";
    for (auto i: dict.items()) {
        cout << count << ". " << i.key();
        if (i.value().is_number()) 
            cout << ": " << i.value();
        cout << "\n";
        count++;
    }
    cout << count << ". Cancel\nEnter: ";
    return count;
}

void printJsonRepeat(json dict, int itr) {
    for (auto i: dict.items()) {
        cout << "\n";
        for (int i = 0; i < itr; i++) {
            cout << "  ";
        }
        cout << i.key() << ": ";
        if (i.value().is_number()) {
            cout << i.value();
        } else {
            printJsonRepeat(dict[i.key()], ++itr);
        }
    }
}

int printJsonAll(json dict) {
    int count = 0;
    for (auto i: dict.items()) {
        cout << "\n" << count++ << ". " << i.key() << ":";
        printJsonRepeat(dict[i.key()], 1);
    }
    cout << "\n" << count << ". Cancel\nEnter: ";
    return count;
}

void update(string link, json file) {
    ofstream ofs(link);
    ofs << file;
    ofs.close();
}

// GAME CLASS
// -----------------------------------------------------

Game::Game()
    : time(new int(0)),
      enumChoice(new int(-1)),
      strChoice(new string("")),
      inputType(new string("")),
      exit(new bool(false)) {
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

int *Game::getTime() {
    return time;
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

void Game::convertInput(json allChoices) {
    if (*strChoice != "") {
        *enumChoice = enumConvert(*strChoice, allChoices);
        *strChoice = "";
    }
}

void Game::convertStrInt() {
    if (*strChoice != "") {
        try {
            for (auto c : *strChoice) {
                if (!isdigit(c))
                    throw 1;
            }
            *enumChoice = stoi(*strChoice);
        } catch (int e) {
            cout << "Did not understand input! "
                    "Please put in a non-negative integer (e.g. 1)."
                    "\n\nEnter: ";
        }
    }
}

void Game::choiceReset() {
    *enumChoice = -1;
    *strChoice = "";
}

json Game::optionsInputType(json options) {
    vector<string> allInputs = {"enumeration", "string", "cancel"};

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
    vector<string> allOptions = { "input type", "cancel" };

    cout << "\nOPTIONS:\n\n";
    ifstream ifs(".json/options.json");
    json options = json::parse(ifs);
    ifs.close();
    printJson(options);

    Game::choiceReset();
    while (true) {
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
                update(".json/options.json", options);
                return;
            default:
                *enumChoice = -1;
                cout << "Not a valid choice!\n\nEnter: ";
            }
        }
    }
}

void Game::manualInner(int choice, ifstream *readme) {
    string line;
    vector<string> allChoices = { "previous", "next", "back" };
    int pages[5][2] = { {13, 62}, {63, 64}, {65, 66}, {67, 68}, {69, 70} };

    readme->clear();
    readme->seekg(0);
    for (int i = 0; i < pages[choice][0]; i++) {
        getline(*readme, line);
    }
    for (int i = pages[choice][0]; i < pages[choice][1]; i++) {
        getline(*readme, line);
        cout << line << "\n";
    }

    cout << "0. Previous\n1. Next\n2. Back\nEnter: ";
    Game::choiceReset();
    while (true) {
        Game::convertInput(allChoices);
        if (*enumChoice != -1) {
            switch (*enumChoice) {
                case 0: // Previous
                    *enumChoice = -1;
                    if (choice == 0) return;
                    else manualInner(--choice, readme);
                    break;
                case 1: // Next
                    *enumChoice = -1;
                    if (choice == allChoices.size()) return;
                    else manualInner(++choice, readme);
                case 2: // Back
                    *enumChoice = -1;
                    return;
                default:
                    *enumChoice = -1;
                    cout << "Not a valid choice!\n\n";
            }
            for (int i = pages[choice][0]; i < pages[choice][1]; i++) {
                getline(*readme, line);
                cout << line << "\n";
            }
            cout << "0. Previous\n1. Next\n2. Back\nEnter: ";
        }
    }
}

void Game::manual() {
    string line;
    ifstream readme(".txt/manual.txt");
    vector<string> allTabs = { "choices", "menus", "controls", "gameplay", "tips", "back" };
    int count = allTabs.size() - 1;

    readme.clear();
    readme.seekg(0);
    for (int i = 0; i < 11; i++) {
        getline(readme, line);
        cout << line << "\n";
    }
    cout << "5. Back\nEnter: ";
    Game::choiceReset();
    while (true) {
        Game::convertInput(allTabs);
        if (*enumChoice != -1) {
            if (*enumChoice < count) {
                Game::manualInner(*enumChoice, &readme);
                readme.clear();
                readme.seekg(0);
                *enumChoice = -1;
            } else if (*enumChoice == count) {
                *enumChoice = -1;
                readme.close();
                return;
            } else {
                *enumChoice = -1;
                cout << "Not a valid choice!\n\n";
            }

            for (int i = 0; i < 11; i++) {
            string line;
            getline(readme, line);
            cout << line << "\n";
            }
            cout << "5. Back\nEnter: ";
        }
    }
}

void Game::moving(XYZ xyz, Character zero) {
    json moves = zero.possibleMoves(xyz);
    vector<string> allMoves, allPoints, allMovement;
    for (auto i : moves.items()) {
        allMoves.push_back(decap(i.key()));
    }
    int count = printJson(moves);

    Game::choiceReset();
    while (true) {
        Game::convertInput(allMoves);
        if (*enumChoice != -1) {
            if (*enumChoice < count) {
                string room = cap(allMoves[*enumChoice]);
                json roomChoice = moves[room];
                count = printJsonAll(roomChoice);
                for (auto i: roomChoice.items()) {
                    allPoints.push_back(decap(i.key()));
                }
                Game::choiceReset();
                while (true) {
                    Game::convertInput(allPoints);
                    if (*enumChoice != -1) {
                        if (*enumChoice < count) {
                            string point = cap(allPoints[*enumChoice]);
                            json pointChoice = roomChoice[point];
                            vector<int> coords = xyz.listRooms()[room][point]["coords"];
                            count = printJson(pointChoice);
                            for (auto i: pointChoice.items()) {
                                allMovement.push_back(decap(i.key()));
                            }
                            Game::choiceReset();
                            while (true) {
                                Game::convertInput(allMovement);
                                if (*enumChoice != -1) {
                                    if (*enumChoice < count) {
                                        string movement = cap(allMovement[*enumChoice]);
                                        int expTime = pointChoice[movement];
                                        cout << "\n" << movement << "...\n";
                                        *enumChoice = -1;
                                        zero.move(room, point, coords, expTime, time);
                                        return;
                                    } else if (*enumChoice == count) {
                                        *enumChoice = -1;
                                        return;
                                    } else {
                                        *enumChoice = -1;
                                        cout << "Not a valid move!\n\n";
                                        printJson(moves);                                        
                                    }                                
                                }
                            }
                        } else if (*enumChoice == count) {
                            *enumChoice = -1;
                            return;
                        } else {
                            *enumChoice = -1;
                            cout << "Not a valid move!\n\n";
                            printJsonAll(moves);
                        }
                    }
                }
            } else if (*enumChoice == count) {
                *enumChoice = -1;
                return;
            } else {
                *enumChoice = -1;
                cout << "Not a valid move!\n\n";
            }
            printJson(moves);
        }
    }
}

void Game::print(string key, int value) {
    vector<string> allChoices = { "yes", "no" };
    string print = "Print where?\n0. Printer Room 1\n"
                    "1. Printer Room 2\n2. Printer Room\n"
                    "3. Cancel\nEnter: ";

    cout << "Send " << key << " to print for " << value << "seconds?\n\n"
                                                      "0. Yes\n1. No\nEnter: ";
    Game::choiceReset();
    while (true) {
        Game::convertInput(allChoices);
        if (*enumChoice != -1) {
            switch (*enumChoice) {
            case 0: // Yes
                Game::choiceReset();
                allChoices = { "printer room 1", "printer room 2", 
                                "printer room 3", "cancel" };
                cout << print;
                while (true) {
                    Game::convertInput(allChoices);
                    if (*enumChoice != -1) {
                        switch(*enumChoice) {
                            case 0:
                            case 1:
                            case 2: 
                                // Send to correct print room printer.
                                *enumChoice = -1;
                                return;
                            case 3: 
                                *enumChoice = -1;
                                return;
                            default:
                                *enumChoice = -1;
                                cout << "Not a valid choice!\n\n";
                        }
                        cout << print;
                    }
                }
            case 1: // No
                *enumChoice = -1;
                return;
            default:
                *enumChoice = -1;
                cout << "Not a valid choice!\n\n";
            }
        }
    }
}

void Game::use(XYZ xyz, Character zero) {
    string *location = zero.getLocation();
}

void Game::printingInner(Character zero, json catalog) {
    vector<string> allItems;
    vector<string> capItems;

    for (auto i : catalog.items()) {
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
                    Game::print(key, value);
                } else {
                    Game::printingInner(zero, value);
                }
            } else if (*enumChoice == count) { // Back
                *enumChoice = -1;
                return;
            } else {
                *enumChoice = -1;
                cout << "Not a valid choice!\n\n";
            }
            count = printJson(catalog);
        }
    }
}

void Game::printing(Character zero) {
    vector<string> allActions = {"tools", "weapons", "cancel" };

    cout << "\n\nPRINT:\n";

    ifstream ifs(".json/items.json");
    json catalog = json::parse(ifs);
    ifs.close();

    Game::printingInner(zero, catalog);
}

void Game::waiting() {
    cout << "Wait for how long?\nEnter: ";

    Game::choiceReset();
    while(true) {
        Game::convertStrInt();
        if (*enumChoice != -1) {
            if (*enumChoice >= 0) {
                cout << "Waiting for " << *enumChoice << " seconds...\n";
                for (int i = 0; i < *enumChoice; i++) {
                    Sleep(1000);
                    *time++;
                    cout << i << "seconds...\n";
                }
                return;
            } else {
                *enumChoice = -1;
                cout << "Not a valid choice!\n";
            }
            cout << "Wait for how long?\nEnter: ";
        }
    }
}

void Game::save(XYZ xyz, vector<Character> characters) {
    string name;
    vector<string> names = { "zero", "one", "two", "three", "four", "five", "zero"};
    json save = {};

    // XYZ Save
    save["xyz"] = {};
    save["xyz"]["config"] = xyz.getConfig();

    // Characters Save
    save["characters"] = {};
    for (int i = 0; i < characters.size(); i++) {
        json charFile = {};
        Character character = characters[i];
        charFile["location"] = *character.getLocation();
        cout << "charFile: " << charFile.dump() << "\n";
        cout << "save: " << save.dump() << "\n\n";
        charFile["point"] = *character.getPoint();
        charFile["x"] = *character.getX();
        charFile["y"] = *character.getY();
        charFile["z"] = *character.getZ();
        charFile["inventory"] = character.getInventory();
        charFile["moveFlag"] = character.getMoveFlag();
        charFile["lookFlag"] = character.getLookFlag();
        charFile["ableFlag"] = character.getAbleFlag();
        save["characters"][names[i]] = charFile;
    }

    save["file"] = true;

    ofstream ofs(".json/save.json");
    ofs << save;
    ofs.close();

    cout << "Saved!\n\n";
    return;
}

// TO DO: Use (Attack?), Print, Automate, Manual <-
// Add Characters and their logic
// Add GUI (OpenGL).

void Game::singlePlayer(XYZ xyz, vector<Character> characters) {
    int univChoice = -1;
    vector<string> allActions = {"move", "look", "use",
                                 "print", "automate", "wait",
                                 "manual", "options", "save", "exit"};
    vector<int>::iterator itr;
    Character zero = characters[0];

    cout << "\nTime: " << to_string(*time) << "\nLocation: "
         << *zero.getLocation() << "\n\n";
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
            case 1: // Look (Add items, characters)
                *enumChoice = -1;
                zero.look(xyz);
                break;
            case 2: // Use (Need to do)
                *enumChoice = -1;
                Game::use(xyz, zero);
                break;
            case 3: // Printing (Make and call printer item)
                *enumChoice = -1;
                Game::printing(zero);
                break;
            case 4: // Automate (Need to do)
                *enumChoice = -1;
                break;
            case 5: // Wait
                *enumChoice = -1;
                Game::waiting();
                break;
            case 6: // Manual
                *enumChoice = -1;
                Game::manual();
                break;
            case 7: // Options
                *enumChoice = -1;
                Game::options();
                break;
            case 8: // Save
                *enumChoice = -1;
                Game::save(xyz, characters);
                break;
            case 9: // Exit
                *enumChoice = -1;
                *exit = true;
                return;
            default:
                *enumChoice = -1;
                cout << "Not a valid choice!\n";
            }

            cout << "\nTime: " << to_string(*time) << "\nLocation: "
                 << *zero.getLocation() << "\n\n";
            zero.printActions(xyz, actions);
        }
    }
}

void Game::load(json xyzFile, json charactersFile) {
    XYZ xyz;
    vector<Character> characters;
    string location, point;

    if (!xyzFile.empty()) {
        xyz.setConfig(xyzFile["config"]);
    }

    if (!charactersFile.empty()) {
        for (auto i: charactersFile.items()) {
            json characterFile = charactersFile[i.key()];
            characters.push_back(Character(xyz, characterFile["location"], characterFile["point"]));
        }
    } else {
        characters.push_back(Character(xyz, "Start 0", "A"));
        characters.push_back(Character(xyz, "Start 1", "B"));
    }
    Game::singlePlayer(xyz, characters);
    return;
}

void Game::newGame() {
    json xyzFile;
    json charactersFile;
    bool flag = true;

    ifstream ifs(".json/characters.json");
    json characters = json::parse(ifs);
    vector<string> allCharacters;

    int count = 0;
    for (auto i: characters.items()) {
        if (!i.value().empty()) {
            cout << count++ << ". " << i.key() << "\n";
            allCharacters.push_back(i.key());
        }
    }
    if (allCharacters.empty()) {
        cout << "\nNo characters to select from...\n\n";
    }
    allCharacters.push_back("start");
    cout << count++ << ". Start\n" << count << ". Cancel\nEnter: ";

    Game::choiceReset();
    while (true) {
        Game::convertInput(allCharacters);
        if (*enumChoice != -1) {
            if (*enumChoice < count - 1) { // Character Select
                string name = characters[cap(allCharacters[*enumChoice])];
                charactersFile[name];
                cout << "Which team? (e.g. 0, 1)\nEnter: ";
                Game::choiceReset();
                while(flag) {
                    Game::convertStrInt();
                    if (*enumChoice != -1) {
                        if (*enumChoice >= 0) {
                            charactersFile[name]["team"] = *enumChoice;
                            *enumChoice = -1;
                            flag = false;
                            break;
                        } else {
                            *enumChoice = -1;
                            cout << "Not a valid option!\n\n";
                            cout << "Which team? (e.g. 0, 1)\nEnter: ";
                        }
                    }
                }

            } else if (*enumChoice == count - 1) { // Start
                *enumChoice = -1;
                Game::load(xyzFile, charactersFile);
                return;
            } else if (*enumChoice == count) { // Cancel
                *enumChoice = -1;
                return;
            } else {
                cout << "Not a valid choice!\n\n";
            }
            count = 0;
            for (auto i: allCharacters) {
                cout << count << ". " << i << "\n"; 
            }
            cout << count++ << ". Start\n" << count << ". Cancel\nEnter: ";
        } 
    }  
}

void Game::resumeGame() {
    ifstream ifs(".json/save.json");
    json save = json::parse(ifs);
    json xyzFile = save["xyz"];
    json charactersFile;
    for (auto i: save["characters"].items()) {
        if (!i.value().empty()) {
            charactersFile[i.key()] = i.value();
        }
    }  
    Game::load(xyzFile, charactersFile);
    return;
}

void Game::singleplayerSelection() {
    ifstream saveIfs(".json/save.json");
    json save = json::parse(saveIfs);
    vector<string> allChoices = { "new", "resume", "cancel" };
    cout << "\n0. New\n1. Resume\n2. Cancel\nEnter: ";

    Game::choiceReset();
    while (true) {
        Game::convertInput(allChoices);
        if (*enumChoice != -1) {
            switch(*enumChoice) {
                case 0: // New
                    *enumChoice = -1;
                    Game::newGame();
                    if (*exit) return;
                    break;
                case 1: // Resume
                    if (!save["file"]) {
                        cout << "No save file found!\n\n";
                        *enumChoice = -1;
                        break;
                    } else {
                        *enumChoice = -1;
                        Game::resumeGame();
                        if (*exit) return;
                        break;
                    }
                case 2: // Cancel
                    *enumChoice = -1;
                    return;
                default:
                    *enumChoice = -1;
                    cout << "Not a valid choice!\n\n";
            }
            cout << "0. New\n1. Resume\n2. Cancel\nEnter: ";
        }
    }
}

void Game::menu() {
    vector<string> allChoices = {"new", "resume", "options",
                                 "manual", "exit" };
    string menuStr = "\n0. Singleplayer\n1. Multiplayer\n2. Options\n3. Manual"
                     "\n4. Exit\nEnter: ";
    cout << menuStr;

    Game::choiceReset();

    while (true) {
        Game::convertInput(allChoices);
        if (*enumChoice != -1) {
            switch (*enumChoice) {
            case 0: // Singleplayer
                *enumChoice = -1;
                Game::singleplayerSelection();
                if (*exit) return;
                break;
            case 1: // Multiplayer
                cout << "Currently unavailable!\n";
                *enumChoice = -1;
                break;
            case 2: // Options
                *enumChoice = -1;
                Game::options();
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
                cout << "Input not in range (e.g. 0-3)!\n";
            }
            cout << menuStr;
        }
    }
}
