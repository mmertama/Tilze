#include <gempyre_utils.h>

#include "tilze.h"
#include "cube.h"
#include "game.h"

Tilze::Tilze(Game& game) :
    m_game(game) {
    m_game.setPostAnimation([this]() {
        GempyreUtils::log(GempyreUtils::LogLevel::Info, "PAA", m_actions.size());
        const auto sz = m_actions.size(); //only actions that are there now
        for(auto i = 0U; i < sz; i++) {
           auto action = m_actions.front();
           m_actions.pop_front();
            GempyreUtils::log(GempyreUtils::LogLevel::Info, "PAB", m_actions.size());
           action();
           GempyreUtils::log(GempyreUtils::LogLevel::Info, "PAC", m_actions.size());
       }
       if(m_squeeze && m_actions.empty())
           squeeze();
       GempyreUtils::log(GempyreUtils::LogLevel::Info, "PAZ", m_actions.size());
    });
}

void Tilze::setHistory(int stripe, int value) {
    m_history = std::make_optional(std::tuple<int, int>{stripe, value});
}

void Tilze::clear() {
    m_cubes.clear();
    m_current_number = 2;
    m_points = 0;
    m_game.draw();
}

Tilze::CubePos Tilze::select(int stripe, int value) {
    m_history = std::nullopt;
    m_selected_stripe = std::make_optional(stripe);
    const auto r = addCube(m_current_number, stripe);
    m_current_number = value;
    return r;
}

Tilze::CubePos Tilze::addCube(int number, int stripe) {
    const auto ani = m_cubes.add(number, stripe);
    if(ani) {
        auto ptr = std::get<CubePtr>(*ani);
        const auto s = std::get<1>(*ani);
        const auto l = std::get<2>(*ani);
        m_actions.push_back([this, s, l, ptr]() {
            merge(ptr, s, l);
        });
    }
    return ani;
}

void Tilze::merge(const CubePtr& cube, int stripe, int level) {
    if(m_squeeze) {

        squeeze();
        m_actions.push_back([cube, stripe, level, this]() {
            merge(cube, stripe, level);
        });
        return;
    }
    if(cube->isAlive()) {
        const auto sisters = m_cubes.takeSisters(*cube, stripe, level);
        if(sisters.size() > 0) {
            const auto value = cube->value();
            m_points += value;
            m_game.setPoints(m_points);
            cube->setValue(value + value);

            for(const auto& sister : sisters) {
                /*
                sister->setPostAnimation([cube, this]() {
                    --m_actives;
                    if(m_actives == 0)
                        squeeze();
                   // merge(cube, stripe, level);
                });*/

                m_squeeze = true;
                m_actions.push_back([cube, stripe, level, this]() {
                    merge(cube, stripe, level);
                });
                m_game.animate(sister, stripe, level);
            }
            //return true;
        }
    }

    if(m_actions.empty() && !m_squeeze) {
        squeeze(); //finish with squeze;
    }


  //  if(!sisters.empty()) {
  //      m_squeeze = true;
  //      m_actions.push_back([](){});
  //   }

   /* const auto merged = !sisters.empty();
    if(merged) {
        m_actions.push_back([this]() {
            squeeze();
        });
    }
    */
 /*   squeeze();
    m_game.requestDraw();
    if(!merged && m_history) {
        m_game.after(200ms, [this]() {
            select(std::get<0>(*m_history), std::get<0>(*m_history));
        });
    }
    if(canAdd() && m_cubes.full()) {
        m_game.setGameOver(m_points);
    }*/
}

void Tilze::squeeze() {
    m_squeeze = false;
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
            const auto moved = m_cubes.move(stripe, level, stripe, next_level); //after  this c is null
            /*++m_actives;
            moved->setPostAnimation([moved, stripe, next_level, this]() {
                --m_actives;
                merge(moved, stripe, next_level);
            });*/
            m_actions.push_back([this, moved, stripe, next_level]() {
                merge(moved, stripe, next_level);
            });
            m_game.animate(moved, stripe, next_level);
        }
    }

    if(m_actions.size() == 3) {
        GempyreUtils::log(GempyreUtils::LogLevel::Info, "squeezed -  here", m_actions.size());
    }

    GempyreUtils::log(GempyreUtils::LogLevel::Info, "squeezed", m_actions.size());

    if(canAdd() && m_cubes.full()) {
        m_game.setGameOver(m_points);
    }
    //return m_actives > 0;
}

bool Tilze::canAdd() const {
    return !m_squeeze && m_actions.empty();
}

