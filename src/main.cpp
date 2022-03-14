#include "tilzeobserver.h"
#include "game.h"
#include "tilze.h"
#include "autoplay.h"
#include <gempyre_utils.h>
#include <gempyre.h>

int main(int argc, char** argv) {
    int auto_data = 0;
    int record = 0;
    for(int i = 1; i < argc; i++) {
        if(argv[i] == std::string("-debug"))
            Gempyre::setDebug();
        if(argv[i] == std::string("-auto") && argc > i + 1)
            auto_data = i + 1;

        if(argv[i] == std::string("-record") && argc > i + 1)
            record = i + 1;
        }
            
    TilzeObserver to;
    Game game(to);

    Tilze tilze(game);
    AutoPlay auto_play(game, tilze);
    to.set(auto_play);
    to.set(tilze);

    if(record)
        tilze.setRecord(argv[record]);

    if(auto_data)
        auto_play.play(argv[auto_data]);

    game.run();
    return 0;
}
