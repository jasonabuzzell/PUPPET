#include "../.hpp/Game.h"
#include "../.hpp/Character.h"
#include "../.hpp/XYZ.h"
#include "../.hpp/Item.h"
#include "../.hpp/Helper.h"
#include "../.hpp/GUI.h"
#include "../.hpp/json.hpp"
#include "../.hpp/glad.h"
#include "../.hpp/glfw3.h"
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <thread>
#include <windows.h>
#include <random>

using namespace std;
using namespace nlohmann;

Game::Game()
    : time(new int(0)),
      enumChoice(new int(-1)),
      strChoice(new string("")),
      inputType(new string("")),
      exit(new bool(false)),
      guiFlag(new bool(false)),
      autosave(true)
{
    cout << "LOADING: [puppet] ... \n";
    "----------------------------\n\n";

    json options = jsonLoad("options");

    if (options["Input Type"] == "Enumeration") {
        *inputType = "Enumeration";
    } else if (options["Input Type"] == "String") {
        *inputType = "String";
    }

    *guiFlag = options["GUI"];

    cout << "Input Type: " << *inputType << "\n";
}

// GAME HELPER FUNCTIONS
// ----------------------------------------------------------------

void guiRun(GUI gui, bool *flag) {
    gui.run(flag);
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

// MOVING
// ----------------------------------------------------------------

json Game::moving(XYZ xyz, Character chara, json actions) {
    json moves = chara.possibleMoves(xyz);
    vector<string> allMoves, allPoints, allMovement;
    for (auto i : moves.items()) {
        allMoves.push_back(decap(i.key()));
    }
    Strint strint = printJson(moves);

    // Choose which room to move to.
    int choice = Game::choice(allMoves, strint.getStr(), strint.getInt());
    if (choice < strint.getInt()) {
        string room = cap(allMoves[choice]);
        json roomChoice = moves[room];
        strint = printJsonAll(roomChoice);
        for (auto i: roomChoice.items()) {
            allPoints.push_back(decap(i.key()));
        }

        // Choose which point to move to.
        choice = Game::choice(allPoints, strint.getStr(), strint.getInt());
        if (choice < strint.getInt()) {
            string point = cap(allPoints[choice]);
            json pointChoice = roomChoice[point];
            vector<float> coords = xyz.listRooms()[room][point]["coords"];
            strint = printJson(pointChoice);
            for (auto i: pointChoice.items()) {
                allMovement.push_back(decap(i.key()));
            }

            // Choose how you move to that point (i.e. walking, running, crouching)
            choice = Game::choice(allMovement, strint.getStr(), strint.getInt());
            if (choice < strint.getInt()) {
                json move;
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

                string coordsStr;
                for (auto f: coords) {
                    coordsStr += roundStr(f) + " "; 
                }
                coordsStr.pop_back();

                actions[chara.getName()]["Move"]["tag"] = "Move: " + room + ": " + point + ": " + coordsStr + ": " + movement;
                return actions; 
            }
        }
    } 
    if (choice == strint.getInt()) return actions;

    return actions;
}

// INTERACT
// ----------------------------------------------------------------

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
                int expTime = movement[cap(allMovement[choice])];
                move["Location"] = *chara.getLocation();
                move["Point"] = item.getPoint();
                move["Current Coords"] = chara.getCoords();
                move["Expected Coords"] = item.getCoords(xyz, chara);
                move["Current Time"] = 0;
                move["Expected Time"] = expTime;

                interact["Type"] = "Take";
                interact["Item Name"] = itemName;
                actions["Active"] = true;
                actions[chara.getName()]["Move"] = move;
                actions[chara.getName()]["Interact"] = interact;

                string coordsStr;
                for (auto f: chara.getCoords()) {
                    coordsStr += roundStr(f) + " "; 
                }
                coordsStr.pop_back();

                string itemCoordsStr;
                for (auto f: item.getCoords(xyz, chara)) {
                    itemCoordsStr += roundStr(f) + " "; 
                }
                itemCoordsStr.pop_back();

                actions[chara.getName()]["Interact"]["tag"] = "Interact: Take: " + itemName + ": " + coordsStr + ": " + itemCoordsStr + ": " + to_string(expTime);

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

// WAITING
// ----------------------------------------------------------------

json Game::waiting(Character chara, json actions) {
    string print = "\nWait for how long?\nEnter: ";

    int choice = Game::choice({}, print);
    actions[chara.getName()]["Wait"] = choice;
    actions[chara.getName()]["Wait"]["tag"] = "Wait: " + to_string(choice);
    actions["Active"] = true;
    return actions;
}

// PRINTING
// ----------------------------------------------------------------

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

json Game::printing(Character chara, json actions) {
    cout << "\n\nPRINT:\n";
    int choice;
    string print;
    json printing;
    json catalog = jsonLoad("items")["Catalog"];

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
    if (choice < allRooms.size()) {
        cout << "Sent " << strint.getStr() << " to print...\n";
        actions[chara.getName()]["Print"]["Item Name"] = strint.getStr();
        actions[chara.getName()]["Print"]["Time"] = strint.getInt();
        actions[chara.getName()]["Print"]["Room"] = allRooms[choice];
        actions["Passive"] = true;
        
        actions[chara.getName()]["Print"]["tag"] = "Print: " + strint.getStr() + ": " + to_string(strint.getInt()) + ": " + allRooms[choice];

        return actions;
    }
    else if (choice == allRooms.size()) return actions; // Cancel

    return actions;
}

// AUTOMATE
// ----------------------------------------------------------------

json Game::automateCompareCoords(XYZ xyz, Character chara, json rooms, vector<string> points, string room) {
    vector<float> newCoords;
    string newPoint;  
    int best = -1;          
    for (string point: points) {
        vector<float> coords = rooms[room][point]["coords"];
        int dist = chara.distance(chara.getCoords(), coords);
        if (best > dist || best == -1) {
            best = dist;
            newCoords = coords;
            newPoint = point; 
        }
    }
    json compare;
    compare["best"] = best;
    compare["newPoint"] = newPoint;
    compare["newCoords"] = newCoords;

    return compare;
}

// Accounts for NPCs in unknown rooms making decisions without randomization. This works because if the character can move, then they will always have the possibility of moving.
json Game::automateActionsMove(XYZ xyz, Character chara, vector<string> parts, json actions) {
    string trueLocation, truePoint;
    vector<float> trueCoords;
    int trueTime;

    string loc = *chara.getLocation();
    string poi = *chara.getPoint();
    json rooms = xyz.listRooms();
    json connect = rooms[loc][poi]["connect"];
    vector<string> points = connect[parts[1]];

    vector<float> expCoords = strVecFloat(splitString(parts[3], " "));
    if (chara.getCoords() != expCoords) {

        if (!connect.contains(parts[1])) { // Unable to reach room.
            vector<float> newCoords;
            string newPoint;  
            int best = -1;
            for (auto i: connect.items()) {
                string room = i.key();
                json compare = Game::automateCompareCoords(xyz, chara, rooms, connect[room], room);
                if (best > compare["best"] || best == -1) {
                    best = compare["best"];
                    trueCoords = {};
                    for (auto f: compare["newCoords"]) {
                        trueCoords.push_back(f);
                    }
                    truePoint = compare["newPoint"];
                    trueLocation = room;
                }
            }
            trueTime = chara.distanceTime(chara.getCoords(), trueCoords)[parts[4]];

        } else if (find(points.begin(), points.end(), parts[2]) == points.end()) { // Unable to reach point.
            json compare = Game::automateCompareCoords(xyz, chara, rooms, connect[parts[1]], parts[1]);
            
            trueLocation = parts[1];
            truePoint = compare["newPoint"];
            for (auto f: compare["newCoords"]) {
                trueCoords.push_back(f);
            }
            trueTime = chara.distanceTime(chara.getCoords(), trueCoords)[parts[4]];

        } else { // Able to reach point.
            trueLocation = parts[1];
            truePoint = parts[2];
            trueCoords = expCoords;
            trueTime = chara.distanceTime(chara.getCoords(), trueCoords)[parts[4]];
        }
        json move;

        move["Location"] = trueLocation;
        move["Point"] = truePoint;
        move["Current Coords"] = chara.getCoords();
        move["Current Time"] = 0;
        move["Expected Coords"] = trueCoords;
        move["Expected Time"] = trueTime;

        actions[chara.getName()]["Move"] = move;
        actions["Active"] = true;
        actions["Pass"] = true;
    }

    return actions;
}

json Game::automateActions(XYZ xyz, vector<Character> charas, Character chara, json actions) {
    json weights;
    weights["Total"] = 0;

    json characters = jsonLoad("characters");
    if (!characters.contains(chara.getName())) return actions;
    json character = characters[chara.getName()];

    bool allActions = character["Auto"];

    for (auto person: charas) {
        json states = json({{"Flags", {}}, {"Locations", {}}});
        vector<vector<string>> labels = {{"Can Look", "Can Not Look"}, {"Can Move", "Can Not Move"}, {"Is Able", "Is Not Able"}};
        vector<bool> flags = {person.getLookFlag(), person.getMoveFlag(), person.getAbleFlag()};
        for (int i = 0; i < flags.size(); i++) {
            if (flags[i]) states["Flags"].push_back(labels[i][0]);
            else states["Flags"].push_back(labels[i][1]);
        }
        string locLabel = *person.getLocation() + ": " + *person.getPoint();
        states["Locations"].push_back(locLabel);

        bool charActions = character[person.getName()]["Auto"];
        bool groupActions;
        bool stateActions;
        
        for (auto i: states.items()) {
            string group = i.key();
            for (string state: states[i.key()]) {
                groupActions = character[person.getName()][group]["Auto"];

                if (character[person.getName()][group].contains(state)) {
                    stateActions = character[person.getName()][group][state]["Auto"];
                    if (allActions || charActions || groupActions || stateActions) {
                        json nextMoves = character[person.getName()][group][state];
                        json clean = nextMoves;
                        clean.erase("Auto");
                        clean.erase("Total");
                        int total = nextMoves["Total"];
                        for (auto move: clean.items()) {
                            float prob = move.value();
                            if (!weights.contains(move.key())) weights[move.key()] = prob;
                            else {
                                float oldProb = weights[move.key()];
                                int oldTotal = weights["Total"];
                                prob = ((oldProb * oldTotal) + (prob * total)) / (oldTotal + total);
                                weights[move.key()] = prob;
                            }
                            weights["Total"] = int(weights["Total"]) + total;
                        }
                    }
                }
            }
        }
    }

    json clean = weights;
    clean.erase("Total");
    if (clean == json({})) return actions;

    float random = float(rand()) / RAND_MAX;
    vector<string> moves = {"Move", "Look", "Interact", "Wait", "Print"};
    
    while (true) { 
        for (auto i: clean.items()) {
            float prob = i.value();
            if (random <= prob) {
                string action = i.key();
                vector<string> parts = splitString(action, ": ");
                auto itr = find(moves.begin(), moves.end(), parts[0]);
                switch (distance(moves.begin(), itr)) {
                    case 0: // Move
                        if (chara.getMoveFlag()) {
                            actions["Pass"] = false;
                            actions = Game::automateActionsMove(xyz, chara, parts, actions);
                            if (actions["Pass"]) return actions;
                        }
                        break;
                    case 1: // Look
                        if (chara.getLookFlag()) {
                            actions = chara.look(xyz, actions);
                            return actions;
                        }
                        break;
                    case 2: // Interact
                        if (chara.getAbleFlag()) {
                            if (parts[1] == "Take") {
                                json move, interact;
                                vector<float> expCoords = strVecFloat(splitString(parts[4], " "));
                                json movement = chara.distanceTime(chara.getCoords(), expCoords);

                                interact["Type"] = parts[1];
                                interact["Item Name"] = parts[2];

                                move["Location"] = *chara.getLocation();
                                move["Point"] = parts[3];
                                move["Current Coords"] = chara.getCoords();
                                move["Current Time"] = 0;
                                move["Expected Coords"] = expCoords;
                                move["Expected Time"] = movement[parts[5]];

                                actions["Active"] = true;
                                actions[chara.getName()]["Move"] = move;
                                actions[chara.getName()]["Interact"] = interact;
                            }
                            return actions;
                        }
                    case 3: // Wait
                        actions[chara.getName()]["Wait"] = parts[1];
                        return actions;
                    case 4: // Print
                        json print;
                        print["Item Name"] = parts[1];
                        print["Time"] = parts[2];
                        print["Room"] = parts[3];
                }
            } 
            random -= prob;
        }
    }

    return actions;
}

void Game::automateUpdate(vector<Character> charas, Character chara, json allStates, string parameter) {
    json characters = jsonLoad("characters");
    string name = chara.getName();
    bool user = chara.getUser();

    if (!characters.contains(name)) {
        characters[name] = json({{"Auto", !user}});
    }

    for (auto character: charas) {
        string bname = character.getName();
        if (!characters[name].contains(bname)) 
            characters[name][bname] = json({{"Auto", !user}});

        if (!characters[name][bname].contains("Flags")) 
            characters[name][bname]["Flags"] = json({{"Auto", !user}});
        
        if (!characters[name][bname].contains("Locations")) 
            characters[name][bname]["Locations"] = json({{"Auto", !user}});

        json results = characters[name][bname];
        for (auto i: allStates.items()) {
            string group = i.key();
            if (!results.contains(group)) results[group] = json({{"Auto", !user}});

            for (string state: allStates[i.key()]) {
                if (!results[group].contains(state)) results[group][state] = json({{"Auto", !user}, {"Total", 0}});
                
                json nextMoves = results[group][state];
                json clean = nextMoves;
                clean.erase("Auto");
                clean.erase("Total");
                clean.erase(parameter);
                int total = nextMoves["Total"];
                float prob;
                if (!nextMoves.contains(parameter)) prob = 0.0;
                else prob = nextMoves[parameter];
                nextMoves[parameter] = (prob * total + 1) / (total + 1);
                for (auto param: clean.items()) {
                    prob = nextMoves[param.key()];
                    nextMoves[param.key()] = (prob * total) / (total + 1);
                }
                nextMoves["Total"] = total + 1;
                results[group][state] = nextMoves;
            }
        }
        characters[name][bname] = results;

    }

    jsonSave(characters, "characters");
}

json Game::automateEnable(json dict) {
    vector<string> allChoices = { "enable", "disable", "back" };
    string print = "\nEnable or Disable Automation:\n0. Enable\n1. Disable\n2. Back\nEnter: ";
    int choice = Game::choice(allChoices, print, allChoices.size());
    
    // For some reason, a switch-case can't handle this.
    if (choice == 0) dict["Auto"] = true; // Enable
    else if (choice == 1) dict["Auto"] = false; // Disable
    
    return dict;
}

void Game::automate(Character chara) {
    int count, choice;
    string print;
    json characters = jsonLoad("characters");
    json character = characters[chara.getName()];

    while (true) {
        vector<string> allChoices = {};
        count = 0;
        for (auto person: character.items()) {
            allChoices.push_back(decap(person.key()));
        }
        allChoices.push_back("back");
        Strint strint = printJson(character);
        print = "\nBased on parameters of:\n";
        choice = Game::choice(allChoices, print + strint.getStr(), strint.getInt());
        string person = cap(allChoices[choice]);

        if (allChoices[choice] == "auto") character = Game::automateEnable(character);
        else if (choice == allChoices.size() - 1) return; // Cancel
        else {
            json groups = character[cap(allChoices[choice])];
            allChoices = {};
            for (auto group: groups.items()) {
                allChoices.push_back(decap(group.key()));
            }
            allChoices.push_back("cancel");
            strint = printJson(groups);
            choice = Game::choice(allChoices, strint.getStr(), strint.getInt());
            string group = cap(allChoices[choice]);

            if (allChoices[choice] == "auto") character[person] = Game::automateEnable(groups);
            else if (choice != allChoices.size() - 1) {
                json params = groups[group];
                count = 0;
                vector<string> allParameters = {};
                for (auto param: params.items()) {
                    allParameters.push_back(decap(param.key()));
                }
                allParameters.push_back("cancel");
                strint = printJson(params);
                choice = Game::choice(allParameters, strint.getStr(), strint.getInt());
                string param = cap(allParameters[choice]);
                
                if (param == "Auto") character[person][group] = Game::automateEnable(params);
                else if (choice != allParameters.size() - 1) {
                    count = 0;
                    print = "\n";
                    json nextMoves = params[param];
                    json clean = nextMoves;
                    clean.erase("Total");
                    vector<string> allActions = {};
                    for (auto i: clean.items()) {
                        allActions.push_back(decap(i.key()));
                    }
                    allActions.push_back("cancel");
                    cout << "size: " << allActions.size() << "\n";
                    Strint strint = printJson(clean);
                    choice = Game::choice(allActions, strint.getStr(), strint.getInt());
                    cout << "choice: " << choice << "\n";
                    string action = cap(allActions[choice]);

                    if (action == "Auto") character[person][person][group][param] = Game::automateEnable(nextMoves);
                    else if (choice != allActions.size() - 1) {
                        float oldFloat = nextMoves[action];
                        int oldInt = round(oldFloat * 100);
                        print = "\nChange the probability of \"" + action + "\" from " + to_string(oldInt) + " to a number between 0 and 100:\nEnter: ";
                        choice = Game::choice({}, print); 
                        if (choice > 100) choice = 100;
                        else if (choice < 0) choice = 0;
                        float choiceFloat = float(choice) / 100;
                        json clean = nextMoves;
                        clean.erase("Auto");
                        clean.erase("Total");
                        clean.erase(action);
                        float oldChoice = nextMoves[action];
                        float oldTotal = 1 - oldChoice;
                        float newTotal = 1 - choiceFloat;
                        int cleanNum = clean.size();
                        float sum = choiceFloat;
                        if (newTotal == 0) {
                            for (auto i: clean.items()) {
                                nextMoves[i.key()] = 0.0;
                            }
                        } else if (oldTotal == 0) {
                            for (auto i: clean.items()) {
                                float calc = newTotal / cleanNum;
                                nextMoves[i.key()] = calc;
                                sum += calc;
                            }
                        } else {
                            for (auto i: clean.items()) {
                                float prob = nextMoves[i.key()];
                                float calc = newTotal * prob / oldTotal;
                                nextMoves[i.key()] = calc;
                                sum += calc;
                            }
                        }
                        if (sum != 1) { // will run regardless because of float inaccuracy.
                            float multi = 1 / sum;
                            choiceFloat *= multi;
                            for (auto i: clean.items()) {
                                float prob = nextMoves[i.key()];
                                nextMoves[i.key()] = prob * multi;
                            }
                        }
                        nextMoves[action] = choiceFloat;
                        character[person][group][param] = nextMoves;
                    }
                }
            }
        }
        characters[chara.getName()] = character; 
        jsonSave(characters, "characters");
    }
}

// MANUAL
// ----------------------------------------------------------------

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

// OPTIONS
// ----------------------------------------------------------------

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
    jsonSave(options, "options");
    
    return options;
}

json Game::optionsAutosave(json options) {
    vector<string> allChoices = {"yes", "no"};
    string print = "\nAUTOSAVE\n:\n\n0. Yes\n1. No\nEnter: ";
    int choice = Game::choice(allChoices, print, allChoices.size());
    switch(choice) {
        case 0: // Yes
            options["Autosave"] = autosave = true;
            break;
        case 1:
            options["Autosave"] = autosave = false;
            break;
        case 2: return options; // Cancel
    }
    jsonSave(options, "options");

    return options;
}

json Game::optionsGUI(json options) {
    vector<string> allChoices = {"yes", "no"};
    string print = "\nGUI:\n\n0. Yes\n1. No\nEnter: ";
    int choice = Game::choice(allChoices, print, allChoices.size());
    switch(choice) {
        case 0: // Yes
            if (!*guiFlag) {
                options["GUI"] = *guiFlag = true;
                threadGUI = new thread(guiRun, gui, guiFlag);
            }
            break;
        case 1:
            if (*guiFlag) {
                options["GUI"] = *guiFlag = false;
                threadGUI->detach();
            }
            break;
        case 2: return options; // Cancel
    }
    jsonSave(options, "options");

    return options;
}

void Game::options() {
    int choice;
    Strint strint("", -1);
    // Relies on Strint to be sorted the same way.
    vector<string> allOptions = { "autosave", "gui", "input type", "cancel" }; 

    cout << "\nOPTIONS:\n";
    json options = jsonLoad("options");

    while (true) {
        strint = printJson(options);
        choice = Game::choice(allOptions, strint.getStr(), strint.getInt());
        switch (choice) {
        case 0: // Autosave
            options = Game::optionsAutosave(options);
            break;
        case 1: // GUI
            options = Game::optionsGUI(options);
            break;
        case 2: // inputType
            options = Game::optionsInputType(options);
            break;
        case 3: return; // Back
        }
    }
}

// SAVE
// ----------------------------------------------------------------

void Game::save(XYZ xyz, vector<Character> characters) {
    string name;
    json charFile;
    json save = json({});

    // Game Save
    save["time"] = *time;
    save["autosave"] = autosave;
    save["gui"] = *guiFlag;

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

    json items = jsonLoad("items")["Location"];
    save["items"] = items;
    jsonSave(save, "save");

    cout << "\nSaved!\n";
}

// ACTIONS
// ----------------------------------------------------------------

// This is essentially a World Clock system for properly correlating actions.
// Only activates when an active user decision is selected
// (i.e. Move, Look, Wait, sometimes Interact)
json Game::actions(XYZ xyz, vector<Character> charas, json actions) {
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

        // Should iterate through actions for each character, when:
        // if (chara.getUser()) actions["Active"] = true;

        for (auto chara: charas) {
            // Gives a chara(cter) its automated moveset.
            actions = Game::automateActions(xyz, charas, chara, actions);
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
                    } else if (chara.getUser()) actions["Active"] = true;
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
                for (auto i: look.items()) {
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
                else {
                    actions[chara.getName()]["Look"] = look;
                    if (chara.getUser()) actions["Active"] = true;
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
                    if (chara.getUser()) actions["Active"] = true;
                    actions[chara.getName()]["Move"] = move;
                }
            } 
            
            if (charActions.contains("Wait")) { // Active
                wait = actions[chara.getName()]["Wait"];
                string plural = " second";
                if (counter != 1) plural += "s";
                cout << "Waiting for " << counter << plural << "...\n";
                if (counter++ == wait) actions[chara.getName()].erase("Wait");
                else if (chara.getUser()) actions["Active"] = true;
            }

            if (actions[chara.getName()].empty()) actions.erase(chara.getName());
        }

        (*time)++;
        Sleep(1000); // Change to smaller increments.
    } 

    return actions;
}

// SINGLEPLAYER
// ----------------------------------------------------------------

void Game::singlePlayer(json file) {
    int choice;
    string print;
    XYZ xyz;
    vector<Character> characters;
    Character zero(xyz, "0", "Start 0", "A", true);
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
        Character one(xyz, "1", "Start 1", "B", false);
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
            characters.push_back(Character(xyz, chara["name"], chara["location"], chara["point"], false));
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

        // Tracks current states for automateUpdate.
        vector<vector<string>> allFlags = {{"Can Move", "Can Not Move"}, {"Can Look", "Can Not Look"}, {"Is Able", "Is Not Able"}};
        vector<bool> charaFlags = {zero.getMoveFlag(), zero.getLookFlag(), zero.getAbleFlag()};
        json allStates;
        vector<string> flags;
        for (int i = 0; i < charaFlags.size(); i++) {
            (charaFlags[i]) ? flags.push_back(allFlags[i][0]) : flags.push_back(allFlags[i][1]);
        }
        allStates["Flags"] = flags;
        allStates["Locations"] = *zero.getLocation() + ": " + *zero.getPoint();

        string tag;
        switch (choice) {
        case 0: // Move
            actions = Game::moving(xyz, zero, actions);
            if (actions["Active"]) {
                tag = actions[zero.getName()]["Move"]["tag"];
                Game::automateUpdate(characters, zero, allStates, tag);
                actions = Game::actions(xyz, characters, actions);
            }
            break;
        case 1: // Look
            actions = zero.look(xyz, actions);
            if (actions["Active"]) {
                Game::automateUpdate(characters, zero, allStates, "Look");
                actions = Game::actions(xyz, characters, actions);
            }
            break;
        case 2: // Interact
            actions = Game::interact(xyz, zero, actions);
            if (actions["Active"]) {
                tag = actions[zero.getName()]["Interact"]["tag"];
                Game::automateUpdate(characters, zero, allStates, tag);
                actions = Game::actions(xyz, characters, actions);
            } else if (actions["Passive"]) {
                tag = actions[zero.getName()]["Interact"]["tag"];
                Game::automateUpdate(characters, zero, allStates, tag);
            }
            break;
        case 3: // Wait
            actions = Game::waiting(zero, actions);
            if (actions["Active"]) {
                tag = actions[zero.getName()]["Wait"]["tag"];
                Game::automateUpdate(characters, zero, allStates, tag);
                actions = Game::actions(xyz, characters, actions);
            }
            break;
        case 4: // Printing
            actions = Game::printing(zero, actions);
            if (actions["Passive"]) {
                actions["Passive"] = false;
                tag = actions[zero.getName()]["Print"]["tag"];
                Game::automateUpdate(characters, zero, allStates, tag);
            }
            break;
        case 5: // Automate
            Game::automate(zero);
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
    string print = "\n";

    json characters = jsonLoad("characters");
    json items = jsonLoad("items");

    // Items handler
    items["Location"] = items["Spawn"];
    jsonSave(items, "items");

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

    // This needs to be fixed now.
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
    json save = jsonLoad("save");
    json items = jsonLoad("items");

    // Game Handler
    *time = save["time"];
    autosave = save["autosave"];
    *guiFlag = save["gui"];

    // Item Handler
    items["Location"] = save["items"];
    jsonSave(items, "items");

    Game::singlePlayer(save);
}

void Game::singleplayerSelection() {
    json save = jsonLoad("save");
    vector<string> allChoices = { "new", "resume", "cancel" };
    string print = "\n0. New\n1. Resume\n2. Cancel\nEnter: ";

    int choice = Game::choice(allChoices, print, allChoices.size());
    switch(choice) {
        case 0: // New
            Game::newGame();
            if (*exit) return;
            break;
        case 1: // Resume
            if (save == json({})) {
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

    if (*guiFlag) threadGUI = new thread(guiRun, gui, guiFlag);

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
            if (*guiFlag) threadGUI->detach();
            return;
        }
    }
}

// TO DO: 
// Add all rooms
// Add all items and functionality (including visibility using objects for hiding, and lethal objects).
// Make sure Characters are automated.
// Remove decap/cap.
// Update GUI.
// Add Audio (Wwise?)