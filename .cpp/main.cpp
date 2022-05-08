#include "../.hpp/Game.h"
#include <iostream>
#include <string>
#include <thread>

using namespace std;

void inputCall(Game game) {
    game.getInput();
}

int main() {
    Game game;
    thread t_input(inputCall, game);
    game.menu();
    t_input.detach();

    return 0;
}