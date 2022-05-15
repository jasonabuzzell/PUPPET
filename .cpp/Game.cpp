#include "../.hpp/Game.h"
#include "../.hpp/Character.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/Item.h"
#include "../.hpp/Helper.h"
#include "../.hpp/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;
using namespace nlohmann;

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

int Game::choice(vector<string> allChoices, string print, int count=0) {
    int choice;
    *enumChoice = -1;
    *strChoice = "";  
    
    cout << print;
    while(true) {
        // String Choice
        if (*strChoice != "") {
            // Number in String Choice
            if (allChoices.empty()) {
                choice = convertStrInt(*strChoice);
                *strChoice = "";
                *enumChoice = -1;
                return choice;
            } else {
                *enumChoice = enumConvert(*strChoice, allChoices);
                *strChoice = "";
            }
        }
        // Enumerator Choice
        if (*enumChoice != -1) {
            // Number in Enumerator Choice
            if (allChoices.empty()) {
                choice = *enumChoice;
                *enumChoice = -1;
                return choice;
            } else if (*enumChoice > count || *enumChoice < 0) {
                *enumChoice = -1;
                cout << "Not a valid choice!\n" << print;
            } else {
                choice = *enumChoice;
                *enumChoice = -1;
                return choice;
            }
        }
    }
}

json Game::optionsInputType(json options) {
    vector<string> allInputs = {"enumeration", "string", "cancel"};
    string print = "\nINPUT TYPE:\n\n0. Enumeration\n1. String\n"
                   "2. Cancel\nEnter: ";

    int choice = Game::choice(allInputs, print, allInputs.size());
    switch (choice) {
    case 0: // Enumerator
        *inputType = "Enumeration";
        options["Input Type"] = "Enumeration";
        break;
    case 1: // String
        *inputType = "String";
        options["Input Type"] = "String";
        break;
    case 2: return options; // Cancel  
    }

    ofstream ofs(".json/options.json");
    ofs << options;
    ofs.close();

    return options;
}

void Game::options() {
    int choice;
    Strint strint("", -1);
    vector<string> allOptions = { "input type", "cancel" };

    cout << "\nOPTIONS:\n";
    ifstream ifs(".json/options.json");
    json options = json::parse(ifs);
    ifs.close();

    while (true) {
        strint = printJson(options);
        choice = Game::choice(allOptions, strint.getStr(), strint.getInt());
        switch (choice) {
        case 0: // inputType
            options = Game::optionsInputType(options);
            break;
        case 1: return; // Back
        }
    }
}

void Game::manualInner(int choose, ifstream *readme) {
    string line;
    string print;
    vector<string> allChoices = { "previous", "next", "back" };
    int pages[5][2] = { {13, 62}, {63, 64}, {65, 66}, {67, 68}, {69, 70} };

    readme->clear();
    readme->seekg(0);
    for (int i = 0; i < pages[choose][0]; i++) {
        getline(*readme, line);
    }
    for (int i = pages[choose][0]; i < pages[choose][1]; i++) {
        getline(*readme, line);
        print += line + "\n";
    }

    print += "0. Previous\n1. Next\n2. Back\nEnter: ";
    int choice = Game::choice(allChoices, print, allChoices.size());
    switch (choice) {
    case 0: // Previous
        if (choice == 0) return;
        else manualInner(--choose, readme);
        break;
    case 1: // Next
        if (choice == allChoices.size()) return;
        else manualInner(++choose, readme);
        break;
    case 2: return; // Back
    }
}

void Game::manual() {
    string line;
    string print;
    ifstream readme(".txt/manual.txt");
    vector<string> allTabs = { "choices", "menus", "controls", "gameplay", "tips", "back" };
    int count = allTabs.size() - 1;

    readme.clear();
    readme.seekg(0);
    for (int i = 0; i < 11; i++) {
        getline(readme, line);
        print += line + "\n";
    }
    print += to_string(count) + ". Back\nEnter: ";
    int choice = Game::choice(allTabs, print, count);
    if (choice < count) {
        Game::manualInner(choice, &readme);
        readme.clear();
        readme.seekg(0);
    } else if (choice == count) {
        readme.close();
        return;
    }
}

void Game::moving(XYZ xyz, Character zero) {
    json moves = zero.possibleMoves(xyz);
    vector<string> allMoves, allPoints, allMovement;
    for (auto i : moves.items()) {
        allMoves.push_back(decap(i.key()));
    }
    Strint strint = printJson(moves);

    int choice = Game::choice(allMoves, strint.getStr(), strint.getInt());
    if (choice < strint.getInt()) {
        string room = cap(allMoves[choice]);
        json roomChoice = moves[room];
        strint = printJsonAll(roomChoice);
        for (auto i: roomChoice.items()) {
            allPoints.push_back(decap(i.key()));
        }

        choice = Game::choice(allPoints, strint.getStr(), strint.getInt());
        if (choice < strint.getInt()) {
            string point = cap(allPoints[choice]);
            json pointChoice = roomChoice[point];
            vector<float> coords = xyz.listRooms()[room][point]["coords"];
            strint = printJson(pointChoice);
            for (auto i: pointChoice.items()) {
                allMovement.push_back(decap(i.key()));
            }
            choice = Game::choice(allMovement, strint.getStr(), strint.getInt());
            if (choice < strint.getInt()) {
                string movement = cap(allMovement[choice]);
                int expTime = pointChoice[movement];
                cout << "\n" << movement << "...\n";
                zero.move(room, point, coords, expTime, time);
                return; 
            }
        }
    } 
    if (choice == strint.getInt()) return;
}

// Needs more work.
void Game::interact(XYZ xyz, Character zero) {
    int roomCount, invCount, choice;
    string itemName;
    vector<string> allItems, allMovement;
    vector<string> roomItems = xyz.getItems(*zero.getLocation());
    vector<string> inventory = xyz.getItems(zero.getName());
    vector<string> allUses = { "use", "drop", "trade" };
    string print = "\nUSE: \n\n" + *zero.getLocation() + ":\n";
    allItems = {};
    roomCount = 0;

    for (auto i: roomItems) {
        print += "  " + to_string(roomCount++) + ". " + i + "\n";
        allItems.push_back(decap(i));
    }
    invCount = roomCount;
    print += "Inventory: \n";
    for (auto j: inventory) {
        print += "  " + to_string(invCount++) + ". " + j + "\n";
        allItems.push_back(decap(j));
    }
    print += "\n" + to_string(invCount) + ". Cancel\nEnter: ";
    
    while (true) {
        choice = Game::choice(allItems, print, invCount);
        if (choice < roomCount) { // item in room (take)
            itemName = cap(allItems[choice]);
            Item item(xyz, *zero.getLocation(), itemName, false);
            json movement = zero.distanceTime(zero.getCoords(), item.getCoords(xyz, zero));
            for (auto i: movement.items()) {
                allMovement.push_back(decap(i.key()));
            }
            string printTwo = "\nTake " + itemName + " by:\n";
            Strint strint = printJson(movement);
            printTwo += strint.getStr();
            choice = Game::choice(allMovement, printTwo, strint.getInt());
            if (choice < strint.getInt()) {
                zero.move(*zero.getLocation(), item.getPoint(), item.getCoords(xyz, zero), movement[cap(allMovement[choice])], time);
                cout << "Taking " + itemName + "...\n";
                Sleep(1000); // Standard 1-second take
                (*time)++;
                item.update(*zero.getLocation(), zero.getName(), "A", true);
                return;
            } // Cancel just goes through the loop again
        } else if (choice < invCount) { // item in inventory (use/drop)
            itemName = cap(allItems[choice]);
            Item item(xyz, zero.getName(), itemName, true);
            string printTwo = "0. Use\n1. Drop\n2. Trade\n3. Cancel\nEnter: "; 
            choice = Game::choice(allUses, printTwo, allUses.size());
            switch (choice) {
                case 0: // Use
                    item.selector();
                    return;
                case 1: // Drop
                    cout << "Dropping " + itemName + "...\n";
                    item.update(zero.getName(), *zero.getLocation(), *zero.getPoint(), false);
                    Sleep(1000);
                    return;
                case 2: // Trade
                    cout << "Not available in singleplayer!\n";
                    break;
                case 3: break; // Cancel
            }
        } else if (choice == invCount) return; // Cancel
    }
}

void Game::print(string key, int value) {
    vector<string> allChoices = { "yes", "no" };
    string print = "Send " + key + " to print for " + to_string(value) + 
                   " seconds?\n\n0. Yes\n1. No\nEnter: ";
    // Go from here.
    int choice = Game::choice(allChoices, print, allChoices.size());
    switch (choice) {
    case 0: // Yes
        allChoices = { "printer room 1", "printer room 2", 
                        "printer room 3", "cancel" };
        print = "Print where?\n0. Printer Room 1\n"
                "1. Printer Room 2\n2. Printer Room\n"
                "3. Cancel\nEnter: ";
        choice = Game::choice(allChoices, print, allChoices.size());
        switch (choice) {
        case 0:
        case 1:
        case 2: 
            // Send to correct print room printer.
            return;
        case 3: return; // Cancel
        }
    case 1: return; // No
    }
}

// Running at O(4i), fix this.
void Game::printingInner(Character zero, json catalog) {
    int choice;
    vector<string> allItems;
    vector<string> capItems;

    for (auto i : catalog.items()) {
        allItems.push_back(decap(i.key()));
        capItems.push_back(i.key()); // forced to do this to access.
    }
    Strint strint = printJson(catalog); // O(2i): Double for-loop.
    while (true) {
        choice = Game::choice(allItems, strint.getStr(), strint.getInt());
        if (choice < strint.getInt()) {
            string key = capItems[choice];
            auto value = catalog[key];
            if (value.is_number()) Game::print(key, value);
            else Game::printingInner(zero, value);
        } else if (choice == strint.getInt()) return; // Back
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
    string print = "\nWait for how long?\nEnter: ";

    int choice = Game::choice({}, print);
    cout << "\nWaiting for " << choice << " seconds...\n";
    for (int i = 1; i <= choice; i++) {
        Sleep(1000);
        (*time)++;
        if (i == 1) cout << i << " second...\n";
        else cout << i << " seconds...\n";
    }

    return;
}

void Game::save(XYZ xyz, vector<Character> characters) {
    string name;
    vector<string> names = { "zero", "one", "two", "three", "four", "five" };
    json save = {};

    // XYZ Save
    save["xyz"] = {};
    save["xyz"]["config"] = xyz.getConfig();

    // Characters Save
    save["characters"] = {};
    for (int i = 0; i < characters.size(); i++) {
        json charFile = {};
        Character character = characters[i];
        charFile["name"] = character.getName();
        charFile["location"] = *character.getLocation();
        charFile["point"] = *character.getPoint();
        charFile["x"] = *character.getX();
        charFile["y"] = *character.getY();
        charFile["z"] = *character.getZ();
        charFile["moveFlag"] = character.getMoveFlag();
        charFile["lookFlag"] = character.getLookFlag();
        charFile["ableFlag"] = character.getAbleFlag();
        save["characters"][names[i]] = charFile;
    }

    // Items Save
    ifstream ifs(".json/items.json");
    json items = json::parse(ifs)["Location"];
    save["items"] = items;

    save["file"] = true;

    ofstream ofs(".json/save.json");
    ofs << save;
    ofs.close();

    cout << "\nSaved!\n\n";
    return;
}

// TO DO: Use (Attack?), Print, Automate, Hiding (in Move?)
// Abstract the functions for any character.
// Add all rooms
// Add Characters and their logic
// Add GUI (OpenGL).
void Game::singlePlayer(XYZ xyz, vector<Character> characters) {
    int choice;
    string print;
    vector<string> allActions = {"move", "look", "interact",
                                 "print", "automate", "wait",
                                 "manual", "options", "save", "exit"};
    vector<int>::iterator itr;
    Character zero = characters[0];

    while (true) {
        print = "\nTime: " + to_string(*time) + "\nLocation: "
                    + *zero.getLocation() + ", Point: " + *zero.getPoint() + "\n\n" + "0. Move\n1. Look\n2. Use\n3. Print\n4. Automate\n5. Wait\n6. Manual\n7. Options\n8. Save\n9. Exit\nEnter: ";
        choice = Game::choice(allActions, print, allActions.size());
        switch (choice) {
        case 0: // Move
            Game::moving(xyz, zero);
            break;
        case 1: // Look (Add items, characters)
            zero.look(xyz);
            break;
        case 2: // Interact (Need to do)
            Game::interact(xyz, zero);
            break;
        case 3: // Printing (Make and call printer item)
            Game::printing(zero);
            break;
        case 4: // Automate (Need to do)
            break;
        case 5: // Wait
            Game::waiting();
            break;
        case 6: // Manual
            Game::manual();
            break;
        case 7: // Options
            Game::options();
            break;
        case 8: // Save
            Game::save(xyz, characters);
            break;
        case 9: // Exit
            *exit = true;
            return;
        }
    }
}

void Game::load(json xyzFile, json charactersFile) {
    XYZ xyz;
    vector<Character> characters;
    string location, point;

    // XYZ
    if (!xyzFile.empty()) {
        xyz.setConfig(xyzFile["config"]);
    }

    if (!charactersFile.empty()) {
        for (auto i: charactersFile.items()) {
            json characterFile = charactersFile[i.key()];
            characters.push_back(Character(xyz, characterFile["name"], characterFile["location"], characterFile["point"]));
        }
    } else {
        characters.push_back(Character(xyz, "0", "Start 0", "A"));
        characters.push_back(Character(xyz, "1", "Start 1", "B"));
    }


    Game::singlePlayer(xyz, characters);
    return;
}

void Game::newGame() {
    json xyzFile;
    json charactersFile;
    string print;
    ifstream ifs(".json/characters.json");
    json characters = json::parse(ifs);
    vector<string> allCharacters;

    int count = 0;
    for (auto i: characters.items()) {
        if (!i.value().empty()) {
            print += to_string(count++) + ". " + i.key() + "\n";
            allCharacters.push_back(i.key());
        }
    }
    if (allCharacters.empty()) {
        print = "\nNo characters to select from...\n\n";
    }
    allCharacters.push_back("start");
    print += to_string(count) + ". Start\n";
    print += to_string(++count) + ". Cancel\nEnter: ";

    // Will need to drop this into an Itr, along with another function.
    while (true) {
        int choice = Game::choice(allCharacters, print, count);
        if (choice < count - 1) { // Character Select
            string name = characters[cap(allCharacters[choice])];
            charactersFile[name];
            print = "Which team? (e.g. 0, 1)\nEnter: ";
            choice = Game::choice({}, print, -1);
            charactersFile[name]["team"] = choice;
        } else if (choice == count - 1) { // Start
            Game::load(xyzFile, charactersFile);
            return;
        } else if (choice == count) return; // Cancel 
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
    string print = "\n0. New\n1. Resume\n2. Cancel\nEnter: ";

    int choice = Game::choice(allChoices, print, allChoices.size());
    switch(choice) {
        case 0: // New
            Game::newGame();
            if (*exit) return;
            break;
        case 1: // Resume
            if (!save["file"]) {
                cout << "No save file found!\n\n";
                break;
            } else {
                Game::resumeGame();
                if (*exit) return;
                break;
            }
        case 2: return; // Cancel
    }
}

void Game::menu() {
    int choice;
    vector<string> allChoices = {"new", "resume", "options",
                                 "manual", "exit" };
    string print = "\n0. Singleplayer\n1. Multiplayer\n2. Options\n3. Manual\n4. Exit\nEnter: ";
    
    while (true) {
        choice = Game::choice(allChoices, print, allChoices.size()-1);
        switch (choice) {
        case 0: // Singleplayer
            Game::singleplayerSelection();
            if (*exit) return;
            break;
        case 1: // Multiplayer
            cout << "Currently unavailable!\n";
            break;
        case 2: // Options
            Game::options();
            break;
        case 3: // Manual
            Game::manual();
            break;
        case 4: // Exit
            *exit = true;
            return;
        }
    }
}
