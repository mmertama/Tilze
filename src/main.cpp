#include <iostream>
#include <cassert>
#include <chrono>
#include <random>

#include "tilze.h"
#include "cube.h"
#include "view.h"
#include "game.h"
#include "autoplay.h"

namespace Gempyre {
    class FrameComposer;
}

static int get2Pow(int max) {
    std::default_random_engine eng(std::random_device {}());
    std::uniform_int_distribution<int> dis(1, max); //not working with old mingw
    const auto exp = dis(eng);
    return static_cast<int>(std::pow(2, exp));
    }


class TilzeObserver : public GameObserver {
   public:

    TilzeObserver& operator=(Tilze& tilze) {m_tilze = &tilze; return *this;}
    TilzeObserver& operator=(AutoPlay& play) {m_play = &play; return *this;}

    void draw(Gempyre::FrameComposer& fc, const View& view) {

        for(const auto& c : *m_tilze) {
            if(!c->isAnimated())
                c->draw(fc);
        }
    }

    void resize(const View& view) {
        const auto h = view.height() / RowCount;
        const auto w = view.stripeInWidth();
        for(auto it = m_tilze->begin() ; it != m_tilze->end(); ++it) {
            const auto x_pos = view.stripePos(m_tilze->stripe(it));
            const auto y_pos = m_tilze->level(it) * h;
                (*it)->setExtents(x_pos, y_pos, w, h);
        }
    }

    int select(int stripe) {
        const int value = get2Pow(6);
        m_tilze->select(stripe, value);
        m_play->add(stripe, value);
        return value;
    }

    void reset() {
         m_tilze->clear();
    }
private:
    Tilze* m_tilze;
    AutoPlay* m_play;
};


int main(int argc, char** argv) {

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
