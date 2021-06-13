#include "tilzeobserver.h"

#include <iostream>
#include <cassert>
#include <chrono>
#include <random>

#include "tilze.h"
#include "cube.h"
#include "view.h"
#include "game.h"
#include "autoplay.h"


static int get2Pow(int max) {
    std::default_random_engine eng(std::random_device {}());
    std::uniform_int_distribution<int> dis(1, max); //not working with old mingw
    const auto exp = dis(eng);
    return static_cast<int>(std::pow(2, exp));
    }



TilzeObserver::TilzeObserver() : m_nextValue(get2Pow(6)) {}
TilzeObserver& TilzeObserver::operator=(Tilze& tilze) {m_tilze = &tilze; return *this;}
TilzeObserver& TilzeObserver::operator=(AutoPlay& play) {m_play = &play; return *this;}

void TilzeObserver::draw(Gempyre::FrameComposer& fc) {
    for(const auto& c : *m_tilze) {
        if(!c->isAnimated())
            c->draw(fc);
    }
}

void TilzeObserver::resize(const View& view)  {
    const auto h = view.height() / RowCount;
    const auto w = view.stripeInWidth();
    for(auto it = m_tilze->begin() ; it != m_tilze->end(); ++it) {
        const auto x_pos = view.stripePos(m_tilze->stripe(it));
        const auto y_pos = m_tilze->level(it) * h;
            (*it)->setExtents(x_pos, y_pos, w, h);
    }
}

GameObserver::CubeInfo TilzeObserver::select(int stripe) {
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

void TilzeObserver::reset()  {
     m_tilze->clear();
}
