#include "tilzeobserver.h"
#include "game.h"
#include "tilze.h"
#include "autoplay.h"
#include <gempyre_utils.h>
#include <gempyre.h>

int main(int argc, char** argv) {
    Gempyre::setDebug();
    TilzeObserver to;
    Game game(to);

    Tilze tilze(game);
    AutoPlay auto_play(game, tilze);
    to = auto_play;
    to = tilze;

    if(argc > 1)
        auto_play.play(argv[1]);

    game.run();
    return 0;
}
