#include "tilzeobserver.h"
#include "game.h"
#include "tilze.h"
#include "autoplay.h"
#include <gempyre_utils.h>
#include <gempyre.h>

int main(int argc, char** argv) {
    int auto_data = 0;
    for(int i = 1; i < argc; i++) {
        if(argv[i] == std::string("-debug"))
            Gempyre::setDebug();
        if(argv[i] == std::string("-auto") && argc < i + 1)
            auto_data = i + 1;    
        }
            
    TilzeObserver to;
    Game game(to);

    Tilze tilze(game);
    AutoPlay auto_play(game, tilze);
    to.set(auto_play);
    to.set(tilze);

    if(auto_data)
        auto_play.play(argv[auto_data]);

    game.run();
    return 0;
}
