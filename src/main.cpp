#include "Game.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "=== Pinaton ===" << std::endl;
        Game game(1280, 720);
        game.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
