#include "../.hpp/Game.h"
#include "../.hpp/GUI.h"
#include <iostream>
#include <string>
#include <thread>

using namespace std;

void inputCall(Game game) {
    game.getInput();
}

void guiCall() {
    GUI gui;
}

int main() {
    Game game;
    thread t_input(inputCall, game);
    thread t_gui(guiCall);
    game.menu();
    t_input.detach();
    t_gui.detach();

    return 0;
}