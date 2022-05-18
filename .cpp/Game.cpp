#include "../.hpp/Game.h"
#include "../.hpp/Character.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/Item.h"
#include "../.hpp/Helper.h"
#include "../.hpp/json.hpp"
#include "../.hpp/glad.h"
#include "../.hpp/glfw3.h"
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <windows.h>

using namespace std;
using namespace nlohmann;

Game::Game()
    : time(new int(0)),
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

void Game::moving(XYZ xyz, Character chara) {
    json moves = chara.possibleMoves(xyz);
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
                chara.move(room, point, coords, expTime, time);
                return; 
            }
        }
    } 
    if (choice == strint.getInt()) return;
}

void Game::interact(XYZ xyz, Character chara) {
    int roomCount, invCount, choice;
    string itemName;
    vector<string> allItems, allMovement;
    vector<string> roomItems = xyz.getItems(*chara.getLocation());
    vector<string> inventory = xyz.getItems(chara.getName());
    vector<string> allUses = { "use", "drop", "trade" };
    string print = "\nUSE: \n\n" + *chara.getLocation() + ":\n";
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
            Item item(xyz, *chara.getLocation(), itemName, false);
            json movement = chara.distanceTime(chara.getCoords(), item.getCoords(xyz, chara));
            for (auto i: movement.items()) {
                allMovement.push_back(decap(i.key()));
            }
            string printTwo = "\nTake " + itemName + " by:\n";
            Strint strint = printJson(movement);
            printTwo += strint.getStr();
            choice = Game::choice(allMovement, printTwo, strint.getInt());
            if (choice < strint.getInt()) {
                chara.move(*chara.getLocation(), item.getPoint(), item.getCoords(xyz, chara), movement[cap(allMovement[choice])], time);
                cout << "Taking " + itemName + "...\n";
                Sleep(1000); // Standard 1-second take
                (*time)++;
                item.update(*chara.getLocation(), chara.getName(), "A", true);
                return;
            } // Cancel just goes through the loop again
        } else if (choice < invCount) { // item in inventory (use/drop)
            itemName = cap(allItems[choice]);
            Item item(xyz, chara.getName(), itemName, true);
            string printTwo = "0. Use\n1. Drop\n2. Trade\n3. Cancel\nEnter: "; 
            choice = Game::choice(allUses, printTwo, allUses.size());
            switch (choice) {
                case 0: // Use
                    item.selector();
                    return;
                case 1: // Drop
                    cout << "Dropping " + itemName + "...\n";
                    item.update(chara.getName(), *chara.getLocation(), *chara.getPoint(), false);
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

// Running at O(4i), fix this.
Strint Game::printingInner(json catalog) {
    int choice;
    vector<string> allItems, capItems;
    Strint strint("", 0);

    for (auto i : catalog.items()) {
        allItems.push_back(decap(i.key()));
        capItems.push_back(i.key()); // forced to do this to access.
    }
    strint = printJson(catalog); // O(2i): Double for-loop.
    while (true) {
        choice = Game::choice(allItems, strint.getStr(), strint.getInt());
        if (choice < strint.getInt()) {
            string key = capItems[choice];
            auto value = catalog[key];
            if (value.is_number()) {
                strint.setStr(key);
                strint.setInt(value);
                return strint;
            } else { 
                strint = Game::printingInner(value);
                if (strint.getInt() != 0) return strint;
            }
        } else if (choice == strint.getInt()) return strint; // Back
    }
}

void Game::print(Character chara, json actions, Strint strint, string room) {
    cout << "Sent " << strint.getStr() << " to print...\n";
    cout << "Actions: " << actions.dump() << "\n";
    actions[chara.getName()]["Print"][room]["Object"] = strint.getStr();
    actions[chara.getName()]["Print"][room]["Time"] = strint.getInt();

    ofstream ofs(".json/actions.json");
    ofs << actions;
    ofs.close();
}

void Game::printing(Character chara) {
    cout << "\n\nPRINT:\n";

    ifstream ifs(".json/items.json");
    json catalog = json::parse(ifs)["Catalog"];
    ifs.close();

    Strint strint = Game::printingInner(catalog);
    if (strint.getInt() == 0) return;
    vector<string> allRooms = {"Printer Room 1", "Printer Room 2", "Cancel"};
    string print = "\nSend " + strint.getStr() + " to print where?\n";
    for (int i = 0; i < allRooms.size(); i++) {
        print += to_string(i) + ". " + allRooms[i] + "\n";
    }
    print += "Enter: ";
    int choice = Game::choice(allRooms, print, allRooms.size());
    if (choice < allRooms.size()) {
        ifstream ifs(".json/actions.json");
        json actions = json::parse(ifs);
        ifs.close();
        if (!actions[chara.getName()].contains("Print")) actions[chara.getName()]["Print"] = json({});
        json prints = actions[chara.getName()]["Print"]; 
        if (prints.contains(allRooms[choice])) {
            vector<string> allChoices = { "yes", "no" };
            string object = prints[allRooms[choice]]["Object"];
            print = object + " is already printing, cancel this job?\n0. Yes\n1. No\nEnter: ";
            int confirm = Game::choice(allChoices, print, allChoices.size());
            if (confirm < allChoices.size()) {
                Game::print(chara, actions, strint, allRooms[choice]);
                return;
            } else if (confirm == allChoices.size()) return;
        } else {
            Game::print(chara, actions, strint, allRooms[choice]);
            return;
        }
    } else if (choice == allRooms.size()) return; // Cancel
}

void Game::automate() {

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
    ifs.close();

    save["file"] = true;

    ofstream ofs(".json/save.json");
    ofs << save;
    ofs.close();

    cout << "\nSaved!\n\n";
    return;
}

// Only activates when an active user decision is selected
// (i.e. Move, Look, Wait, sometimes Interact)
void Game::actions(XYZ xyz, Character chara) {
    ifstream ifs(".json/actions.json");
    json actions = json::parse(ifs);
    ifs.close();

    *enumChoice = -1;
    *strChoice = "";
    cout << "\nEnter anything to cancel action...";
    while (true) {
        if (*enumChoice != -1 || *strChoice != "") {
            cout << "Cancelling action...\n";
            return;
        } 
        if (actions[chara.getName()] != json({})) {
            for (auto i: actions[chara.getName()].items()) {
                if (i.key() == "Print") { // Passive
                    json prints = actions[chara.getName()]["Print"];
                    for (auto j: prints.items()) {
                        string itemName = prints[j.key()]["Object"];
                        int time = prints[j.key()]["Time"];
                        if (time <= 0) {
                            actions[chara.getName()]["Print"].erase(j.key());
                            ifstream ifs(".json/items.json");
                            json items = json::parse(ifs);
                            ifs.close();
                            if (!items["Location"].contains(j.key())) items["Location"][j.key()] = json({});
                            items["Location"][j.key()][itemName] = "Printer";
                            Item item(xyz, j.key(), itemName, false);

                            ofstream ofs(".json/items.json");
                            ofs << items;
                            ofs.close();
                        } else {
                            actions[chara.getName()]["Print"][j.key()]["Time"] = --time;
                        }
                    }
                }

                if (i.key() == "Interact") { // Active / Passive

                }

                
                if (i.key() == "Look") { // Active

                } else if (i.key() == "Move") { // Active

                } else if (i.key() == "Wait") { // Active

                }
            }
        }
        Sleep(1000);
    }

    ofstream ofs(".json/actions.json");
    ofs << actions;
    ofs.close();
}

// TO DO: Automate, Visibility (for hiding), 
// Fix little issue where items can't be found after save (might think it's carried)
// Add all rooms
// Add all items and functionality
// Add Characters and their logic
// Add GUI (OpenGL)
// Add Audio (Wwise?)
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
            Game::actions(xyz, zero);
            break;
        case 1: // Look (Add items, characters)
            zero.look(xyz);
            Game::actions(xyz, zero);
            break;
        case 2: // Interact
            Game::interact(xyz, zero);
            Game::actions(xyz, zero);
            break;
        case 3: // Printing
            Game::printing(zero);
            break;
        case 4: // Automate (Need to do)
            Game::automate();
            break;
        case 5: // Wait
            Game::waiting();
            Game::actions(xyz, zero);
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

    // Characters
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
    vector<string> allCharacters;
    string print;

    ifstream ifsc(".json/characters.json");
    json characters = json::parse(ifsc);
    ifsc.close();
    ifstream ifsi(".json/items.json");
    json items = json::parse(ifsi);
    ifsi.close();
    ifstream ifsa(".json/actions.json");
    json actions = json::parse(ifsa);
    ifsa.close();

    // Actions handler
    actions = json({});
    ofstream ofsa(".json/actions.json");
    ofsa << actions;
    ofsa.close();

    // Items handler
    items["Location"] = items["Spawn"];
    ofstream ofsi(".json/items.json");
    ofsi << items;
    ofsi.close();

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
    ifstream ifss(".json/save.json");
    json save = json::parse(ifss);
    ifss.close();
    ifstream ifsi(".json/items.json");
    json items = json::parse(ifsi);
    ifsi.close();

    // Item Handler
    items["Location"] = save["items"];
    ofstream ofsi(".json/items.json");
    ofsi << items;
    ofsi.close();

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

// Need to add all Game threads.
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
