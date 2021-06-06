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

    TilzeObserver() : m_nextValue(get2Pow(6)) {}
    TilzeObserver& operator=(Tilze& tilze) {m_tilze = &tilze; return *this;}
    TilzeObserver& operator=(AutoPlay& play) {m_play = &play; return *this;}

    void draw(Gempyre::FrameComposer& fc) override {
        for(const auto& c : *m_tilze) {
            if(!c->isAnimated())
                c->draw(fc);
        }
    }

    void resize(const View& view) override {
        const auto h = view.height() / RowCount;
        const auto w = view.stripeInWidth();
        for(auto it = m_tilze->begin() ; it != m_tilze->end(); ++it) {
            const auto x_pos = view.stripePos(m_tilze->stripe(it));
            const auto y_pos = m_tilze->level(it) * h;
                (*it)->setExtents(x_pos, y_pos, w, h);
        }
    }

    GameObserver::CubeInfo select(int stripe) override {
        auto ptr = m_tilze->select(stripe, m_nextValue);
        if(ptr)
        {
            m_play->add(stripe, m_nextValue);
            const auto r_value = std::make_optional<GameObserver::CubeInfo::value_type> ({
                                                                                             std::get<0>(*ptr),
                                                                                             std::get<1>(*ptr),
                                                                                             std::get<2>(*ptr),
                                                                                             m_nextValue});
            m_nextValue = get2Pow(6);
            return r_value;
        }
       return std::nullopt;
    }

    void reset() override {
         m_tilze->clear();
    }

private:
    Tilze* m_tilze = nullptr;
    AutoPlay* m_play = nullptr;
    int m_nextValue;
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
