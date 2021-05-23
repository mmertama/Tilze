#include <gempyre_utils.h>

#include "tilze.h"
#include "cube.h"
#include "game.h"

Tilze::Tilze(Game& game) :
    m_game(game) {
}


bool Tilze::isActive() const {
    return m_active;
}

void Tilze::setHistory(int stripe, int value) {
    m_history = std::make_optional(std::tuple<int, int>{stripe, value});
}

int Tilze::current() const {
    return m_current_number;
}


void Tilze::clear() {
    m_cubes.clear();
    m_current_number = 2;
    m_points = 0;
    m_game.draw();
}

void Tilze::select(int stripe, int value) {
    m_history = std::nullopt;
    m_selected_stripe = std::make_optional(stripe);
    addCube(m_current_number, stripe, RowCount + 1);
     m_game.draw();
    m_current_number = value;
}

void Tilze::addCube(int number, int stripe, int level) {
    //std::cerr << "addCube" << m_onRedraw << std::endl;
    const auto ani = m_cubes.add(number, stripe);
    if(ani) {
        m_active = true;
        squeeze();
       /* const Stripes stripes(m_width);
        const auto xpos = stripes.stripePos(stripe);
        //-1 as this one was added
        const auto ypos = yPos(m_cubes.level(stripe) - 1);
        const auto period = time(m_height, ypos);
        ani->animate(xpos, start_y, xpos, ypos, period, [ani, this]() {
            merge(ani);
            });
        m_animator.addAnimation(ani);*/
    }
}

void Tilze::merge(const CubePtr& cube, int stripe, int level) {
    if(!cube->isAlive())
        return;
    //std::cerr << "merge" << m_onRedraw << std::endl;
    auto sisters = m_cubes.takeSisters(*cube, stripe, level);
    if(sisters.size() > 0) {
        const auto value = cube->value();
        m_points += value;
        m_game.setPoints(m_points);
        cube->setValue(value + value);

        for(auto& sister : sisters) {
            m_game.animate(sister, cube->x(), cube->y(), [&, this] () {
                merge(cube, stripe, level);
            });
        }
        return;
    }
    const auto merged = !sisters.empty();
    squeeze();
    requestDraw();
    if(!merged && m_history) {
        m_game.after(200ms, [this]() {
            select(std::get<0>(*m_history), std::get<0>(*m_history));
        });
    }
    if(!m_active && m_cubes.full()) {
        m_game.setGameOver(m_points);
    }
}

void Tilze::squeeze() {
    assert(m_active);
    GempyreUtils::log(GempyreUtils::LogLevel::Info, "squeeze");
    for(auto it = begin(); it != end(); ++it)  {
        const auto level = this->level(it);
        auto next_level = level;
        const auto c = *it;
        const auto stripe = this->stripe(it);
        while(next_level > 0 && !m_cubes.has(stripe, next_level - 1)) {
            --next_level; //how up to go
        }
        if(next_level < level) {
            auto moved = m_cubes.move(stripe, next_level); //after  this c is null
            m_game.animate(moved, stripe, next_level, [&, this]() {
                merge(moved, stripe, next_level);
            });
        }
    }
    GempyreUtils::log(GempyreUtils::LogLevel::Info, "squeezed");
}


/*
int Tilze::level(const Cube& cube) const {
    const auto begin = pos(cube.stripe(), 0);
    const auto end = begin + RowCount;
     for(auto it = begin; it != end; ++it)
        if(&cube == m_cubes[it].get())
            return it - begin;
    assert(false);
    return -1;
    }
    */

/*
void Tilze::requestDraw() {
    GempyreUtils::log(GempyreUtils::LogLevel::Info, "requestDraw", m_onRedraw);
    if(!m_onRedraw) {
        m_onRedraw = true;

        mAfter(0ms, [this]() {
            GempyreUtils::log(GempyreUtils::LogLevel::Info, "requestDraw - act", m_animator.isActive());
            if(!m_animator.isActive()) {
                draw();
                m_onRedraw = false;
            }
        });
    }
}
*/
