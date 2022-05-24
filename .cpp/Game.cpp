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
      exit(new bool(false)),
      autosave(true) 
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

// moving() function has to be here because it involves Game::choice(),
// meaning that it involves user input as a game decision. That's why
// look() exists as a Character function because it does not need input.
json Game::moving(XYZ xyz, Character chara, json actions) {
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
                json move = json({});
                string movement = cap(allMovement[choice]);
                int expTime = pointChoice[movement];
                move["Location"] = room;
                move["Point"] = point;
                move["Current Coords"] = chara.getCoords();
                move["Expected Coords"] = coords;
                move["Current Time"] = 0;
                move["Expected Time"] = expTime;
                actions[chara.getName()]["Move"] = move;
                actions["Active"] = true;
                return actions; 
            }
        }
    } 
    if (choice == strint.getInt()) return actions;

    return actions;
}

json Game::interact(XYZ xyz, Character chara, json actions) {
    int roomCount, invCount, choice;
    string itemName;
    json interact, move;

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
            Item item(*chara.getLocation(), itemName, false);
            json movement = chara.distanceTime(chara.getCoords(), item.getCoords(xyz, chara));
            for (auto i: movement.items()) {
                allMovement.push_back(decap(i.key()));
            }
            string printTwo = "\nTake " + itemName + " by:\n";
            Strint strint = printJson(movement);
            printTwo += strint.getStr();
            choice = Game::choice(allMovement, printTwo, strint.getInt());
            if (choice < strint.getInt()) { 
                // Take uses Move to get to item.
                move["Location"] = *chara.getLocation();
                move["Point"] = item.getPoint();
                move["Current Coords"] = chara.getCoords();
                move["Expected Coords"] = item.getCoords(xyz, chara);
                move["Current Time"] = 0;
                move["Expected Time"] = movement[cap(allMovement[choice])];

                interact["Type"] = "Take";
                interact["Item Name"] = itemName;
                actions["Active"] = true;
                actions[chara.getName()]["Move"] = move;
                actions[chara.getName()]["Interact"] = interact;
                return actions;
            } // Cancel just goes through the loop again
        } else if (choice < invCount) { // item in inventory (use/drop)
            itemName = cap(allItems[choice]);
            Item item(chara.getName(), itemName, true);
            string printTwo = "0. Use\n1. Drop\n2. Trade\n3. Cancel\nEnter: "; 
            choice = Game::choice(allUses, printTwo, allUses.size());
            switch (choice) {
                case 0: // Use
                    item.selector();
                    interact["Type"] = "Use";
                    actions["Active"] = true;
                    return actions;
                case 1: // Drop
                    interact["Type"] = "Drop";
                    actions["Active"] = true;
                    return actions;
                case 2: // Trade
                    cout << "Not available in singleplayer!\n";
                    break;
                case 3: break; // Cancel
            }
        } else if (choice == invCount) return actions; // Cancel
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

json Game::print(Character chara, json actions, Strint strint, string room) {
    cout << "Sent " << strint.getStr() << " to print...\n";
    actions[chara.getName()]["Print"]["Item Name"] = strint.getStr();
    actions[chara.getName()]["Print"]["Time"] = strint.getInt();
    actions[chara.getName()]["Print"]["Room"] = room;

    return actions;
}

json Game::printing(Character chara, json actions) {
    cout << "\n\nPRINT:\n";
    int choice;
    string print;
    json printing;
    ifstream ifs(".json/items.json");
    json catalog = json::parse(ifs)["Catalog"];
    ifs.close();

    if (actions[chara.getName()].contains("Print")) {
        printing = actions[chara.getName()]["Print"];
        cout << "Currently printing: " << printing["Item Name"] << 
        " Time left: " << printing["Time"] << "\n";
    } 

    Strint strint = Game::printingInner(catalog);
    if (strint.getInt() == 0) return actions;

    if (actions[chara.getName()].contains("Print")) { // Not particularly in love with this double check.
        vector<string> allChoices = { "yes", "no" };
        print = "Printing this item will cancel your previous print. Continue?\n\n0. Yes\n1. No\nEnter: ";
        choice = Game::choice(allChoices, print, allChoices.size());
        if (choice == 1) return actions;
    }

    vector<string> allRooms = {"Printer Room 1", "Printer Room 2", "Cancel"};
    print = "\nSend " + strint.getStr() + " to print where?\n";
    for (int i = 0; i < allRooms.size(); i++) {
        print += to_string(i) + ". " + allRooms[i] + "\n";
    }
    print += "Enter: ";
    choice = Game::choice(allRooms, print, allRooms.size());
    if (choice < allRooms.size()) return Game::print(chara, actions, strint, allRooms[choice]);
    else if (choice == allRooms.size()) return actions; // Cancel

    return actions;
}

void Game::automate() {

}

json Game::waiting(Character chara, json actions) {
    string print = "\nWait for how long?\nEnter: ";

    int choice = Game::choice({}, print);
    actions[chara.getName()]["Wait"] = choice;
    actions["Active"] = true;
    return actions;
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

json Game::optionsAutosave(json options) {
    vector<string> allChoices = {"yes", "no"};
    string print = "\nAUTOSAVE:\n\n0. Yes\n1. No\nEnter: ";
    int choice = Game::choice(allChoices, print, allChoices.size());
    switch(choice) {
        case 0: // Yes
            autosave = true;
            options["Autosave"] = "Yes";
            break;
        case 1:
            autosave = false;
            options["Autosave"] = "No";
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
    vector<string> allOptions = { "input type", "autosave" "cancel" };

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
        case 1: // Autosave
            options = Game::optionsAutosave(options);
            break;
        case 2: return; // Back
        }
    }
}

// May be out of order
void Game::save(XYZ xyz, vector<Character> characters) {
    string name;
    json charFile;
    json save = json({});

    // Game Save
    save["time"] = *time;
    save["autosave"] = save;

    // XYZ Save
    save["xyz"] = {};
    save["xyz"]["config"] = xyz.getConfig();

    // Characters Save
    save["characters"] = json({});
    for (auto character: characters) {
        charFile = json({});
        charFile["name"] = character.getName();
        charFile["location"] = *character.getLocation();
        charFile["point"] = *character.getPoint();
        charFile["x"] = *character.getX();
        charFile["y"] = *character.getY();
        charFile["z"] = *character.getZ();
        charFile["moveFlag"] = character.getMoveFlag();
        charFile["lookFlag"] = character.getLookFlag();
        charFile["ableFlag"] = character.getAbleFlag();
        save["characters"][character.getName()] = charFile;
    }

    save["file"] = true;
    ifstream ifs(".json/items.json");
    json items = json::parse(ifs)["Location"];
    save["items"] = items;
    ifs.close();
    ofstream ofs(".json/save.json");
    ofs << save;
    ofs.close();

    cout << "\nSaved!\n";
}

// This is essentially a World Clock system for properly correlating actions.
// Only activates when an active user decision is selected
// (i.e. Move, Look, Wait, sometimes Interact)
// Should also update character preferences.
json Game::actions(XYZ xyz, Character chara, json actions) {
    json charActions;
    int timer, wait;
    *enumChoice = -1;
    *strChoice = "";
    cout << "\nEnter anything to cancel action...\n";
    int count, counter = 0;

    // Check if there are any active actions left.
    while (actions["Active"]) {
        if (*enumChoice != -1 || *strChoice != "") {
            cout << "Cancelling action...\n";
            return actions;
        } 
        actions["Active"] = false;
        charActions = actions[chara.getName()];

        if (charActions.contains("Print")) { // Passive
            json printing = actions[chara.getName()]["Print"];
            string room = printing["Room"];
            string itemName = printing["Item Name"];
            timer = printing["Time"];
            if (timer < 1) { // Adds item to XYZ
                actions[chara.getName()].erase("Print");
                addItem(itemName, room, "Printer");
            } else actions[chara.getName()]["Print"]["Time"] = --timer;
        }

        if (charActions.contains("Interact")) { // Active / Passive
            json interact = actions[chara.getName()]["Interact"];
            string itemName = interact["Item Name"];
            Item item(*chara.getLocation(), itemName, false);
            if (interact["Type"] == "Take") {
                cout << "Taking " << itemName << "...\n";
                if (!charActions.contains("Move")) { 
                    item.update(*chara.getLocation(), chara.getName(), "A", true);
                    actions[chara.getName()].erase("Interact");
                } else actions["Active"] = true;
            } else if (interact["Type"] == "Drop") {
                cout << "Dropping " << itemName << "...\n";
                item.update(chara.getName(), *chara.getLocation(), *chara.getPoint(), false);
                actions[chara.getName()].erase("Interact");
            } else if (interact["Type"] == "Use") {
                cout << "Using " << itemName << "...\n";
                // Have to add more to check item usage.
                actions[chara.getName()].erase("Interact");
            }
        }

        
        if (charActions.contains("Look")) { // Active
            cout << "Looking...\n";
            json look = actions[chara.getName()]["Look"];
            for (auto i: look.items()) { // That's nasty i.key() replacement.
                timer = look[i.key()]["Time"];
                if (timer < 1) {
                    look[i.key()].erase("Time");
                    if (i.key() == "Rooms") {
                        cout << "  " << i.key() << ":\n" << jsonListPrint(look[i.key()]);
                    } else {
                        cout << "  " << i.key() << ": " << listPrint(look[i.key()]["Items"]);
                    }
                    look.erase(i.key());
                } else look[i.key()]["Time"] = --timer;
            }
            if (look == json({})) actions[chara.getName()].erase("Look");
            else { // If "look" stills exists, then there are active actions.
                actions[chara.getName()]["Look"] = look;
                actions["Active"] = true;
            }

        } 
        
        if (charActions.contains("Move")) { // Active
            cout << "\nMoving...\n";
            json move = actions[chara.getName()]["Move"];
            chara.move(move["Current Coords"], move["Expected Coords"], move["Current Time"], move["Expected Time"]);
            move["Current Time"] = int(move["Current Time"]) + 1;
            if (move["Current Time"] == move["Expected Time"]) {
                chara.setLocation(move["Location"]);
                chara.setPoint(move["Point"]);
                chara.setCoords(move["Expected Coords"]);
                actions[chara.getName()].erase("Move");
            } else { 
                actions["Active"] = true;
                actions[chara.getName()]["Move"] = move;
            }
        } 
        
        if (charActions.contains("Wait")) { // Active
            wait = actions[chara.getName()]["Wait"];
            string plural = " second";
            if (counter != 1) plural += "s";
            cout << "Waiting for " << counter << plural << "...\n";
            if (counter++ == wait) actions[chara.getName()].erase("Wait");
            else actions["Active"] = true;
        }

        if (actions[chara.getName()].empty()) actions.erase(chara.getName());
        (*time)++;
        Sleep(1000); // Might change this later to smaller time segments.
    } 
    return actions;
}

// TO DO: 
// Visibility (for hiding)
// Add all rooms
// Add all items and functionality
// Add Characters and their logic with automate().
// Add GUI (OpenGL)
// Add Audio (Wwise?)
void Game::singlePlayer(json file) {
    int choice;
    string print;
    XYZ xyz;
    vector<Character> characters;
    Character zero(xyz, "0", "Start 0", "A");
    json actions = json({});
    actions["Active"] = false;
    vector<string> allActions = {"move", "look", "interact",
                                 "wait", "print", "automate",
                                 "manual", "options", "save", "exit"};
    vector<int>::iterator itr;
    vector<string> strCoords;

    // XYZ Handler
    if (file.contains("xyz")) xyz.setConfig(file["xyz"]["config"]);

    // Character Handler
    if (file["characters"] == json({})) {
        Character one(xyz, "1", "Start 1", "B");
        characters.push_back(zero);
        characters.push_back(one);
    } else {
        if (file["characters"].contains("0")) { // Update 0
            json zeroFile = file["characters"]["0"];
            vector<string> position = {zeroFile["location"], zeroFile["point"]};
            vector<float> coords = {zeroFile["x"], zeroFile["y"], zeroFile["z"]};
            vector<bool> flags = {zeroFile["moveFlag"], zeroFile["ableFlag"], zeroFile["lookFlag"]};
            zero.setParameters(position, coords, flags);
        }
        for (auto chara: file["characters"]) {
            characters.push_back(Character(xyz, chara["name"], chara["location"], chara["point"]));
        }
    }

    // Singleplayer Handler
    while (true) {
        strCoords = {};
        for (auto i: zero.getCoords()) {
            strCoords.push_back(roundStr(i));
        }

        print = "\nTime: " + to_string(*time) + "\nLocation: "
                + *zero.getLocation() + ", Point: " + *zero.getPoint() + ", X: " + strCoords[0] + " Y: " + strCoords[1] + " Z: " + strCoords[2] + "\n\n" + "0. Move\n1. Look\n2. Interact\n3. Wait\n4. Print\n5. Automate\n6. Manual\n7. Options\n8. Save\n9. Exit\nEnter: ";
        choice = Game::choice(allActions, print, allActions.size());
        switch (choice) {
        case 0: // Move
            actions = Game::moving(xyz, zero, actions);
            if (actions["Active"]) actions = Game::actions(xyz, zero, actions);
            break;
        case 1: // Look (Add items, characters)
            actions = zero.look(xyz, actions);
            if (actions["Active"]) actions = Game::actions(xyz, zero, actions);
            break;
        case 2: // Interact
            actions = Game::interact(xyz, zero, actions);
            if (actions["Active"]) actions = Game::actions(xyz, zero, actions);
            break;
        case 3: // Wait
            actions = Game::waiting(zero, actions);
            if (actions["Active"]) actions = Game::actions(xyz, zero, actions);
            break;
        case 4: // Printing
            actions = Game::printing(zero, actions);
            break;
        case 5: // Automate (Need to do)
            Game::automate();
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
            if (autosave) Game::save(xyz, characters);
            *exit = true;
            return;
        }
    }
}

void Game::newGame() {
    json file;
    vector<string> allCharacters;
    string print;

    ifstream ifsc(".json/characters.json");
    json characters = json::parse(ifsc);
    ifsc.close();
    ifstream ifsi(".json/items.json");
    json items = json::parse(ifsi);
    ifsi.close();

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
    file["characters"] = json({});
    print += to_string(count) + ". Start\n";
    print += to_string(++count) + ". Cancel\nEnter: ";

    // Will need to drop this into an Itr, along with another function.
    while (true) {
        int choice = Game::choice(allCharacters, print, count);
        if (choice < count - 1) { // Character Select
            string name = characters[cap(allCharacters[choice])];
            file["characters"][name] = json({});
            print = "Which team? (e.g. 0, 1)\nEnter: ";
            choice = Game::choice({}, print, -1);
            file["characters"][name]["team"] = choice;
        } else if (choice == count - 1) { // Start
            Game::singlePlayer(file);
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

    // Game Handler
    *time = save["time"];
    autosave = save["autosave"];

    // Item Handler
    items["Location"] = save["items"];
    ofstream ofsi(".json/items.json");
    ofsi << items;
    ofsi.close();

    Game::singlePlayer(save);
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
