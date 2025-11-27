#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;
    if (game.init("NeoBall Replica", 800, 600)) {
        game.run();
    }
    return 0;
}  