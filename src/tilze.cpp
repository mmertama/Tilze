#include <gempyre_utils.h>

#include "tilze.h"
#include "cube.h"
#include "game.h"

Tilze::Tilze(Game& game) :
    m_game(game) {
    m_game.setPostAnimation([this]() {
        const auto sz = m_actions.size(); //only actions that are there now
        for(auto i = 0U; i < sz; i++) {
           auto action = m_actions.front();
           m_actions.pop_front();
           action();
       }
       if(m_squeeze && m_actions.empty())
           squeeze();
    });
}

void Tilze::setHistory(int stripe, int value) {
    m_history = std::make_optional(std::tuple<int, int>{stripe, value});
}

void Tilze::clear() {
    m_cubes.clear();
    m_current_number = 2;
    m_points = 0;
    m_game.drawOnce();
}

std::optional<Tilze::CubePtr> Tilze::select(int stripe, int value) {
    m_history = std::nullopt;
    m_selected_stripe = std::make_optional(stripe);
    const auto r = addCube(m_current_number, stripe);
    if(r) {
        record(m_current_number, stripe);
    }
    m_current_number = value;
    return r;
}


std::optional<Tilze::CubePtr> Tilze::addCube(int number, int stripe) {
    const auto ani = m_cubes.add(number, stripe);
    if(ani) {
        m_actions.push_back([this, ani]() {
            merge(*ani);
        });
    }
    return ani;
}

std::optional<std::tuple<int, int>> Tilze::position(const CubePtr& ptr) {
    const auto p = m_cubes.find(ptr);
    return p != end()
            ? std::make_optional(std::make_tuple(m_cubes.column(p), m_cubes.row(p))) : std::nullopt;
}

void Tilze::merge(const CubePtr& cube) {
    squeeze();
    if(cube->isAlive()) {
        const auto sisters = m_cubes.takeSisters(cube);
        if(sisters.size() > 0) {
            const auto value = cube->value();
            m_points += value;
            m_game.setPoints(m_points);
            //cube->setValue(value + value);
            for(const auto& sister : sisters) {
                m_game.animate(sister, Game::Animation::Fade);
            }
            //for(const auto& sister : sisters) {
                /*
                sister->setPostAnimation([cube, this]() {
                    --m_actives;
                    if(m_actives == 0)
                        squeeze();
                   // merge(cube, stripe, level);
                });*/

                m_squeeze = true;
                m_actions.push_back([cube, this]() {
                    cube->setValue(cube->value() * 2);
                    merge(cube);
               });
                m_game.animate(cube, Game::Animation::Move);
              //  m_game.animate(sister);
          //  }
            //return true;
        }
    }


   // if(m_actions.empty() && !m_squeeze) {
   //     squeeze(); //finish with squeze;
   // }


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
/*
bool Tilze::needsSqueeze(const CubePtr& cube) const {
    return cubee->level() > 0 && !m_cubes.has(stripe, level - 1))
}*/



void Tilze::squeeze() {
    m_squeeze = false;
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
            m_actions.push_back([this, moved]() {
                merge(moved);
            });
            m_game.animate(moved, Game::Animation::Move);
        }
    }

    if(canAdd() && m_cubes.full()) {
        m_game.setGameOver(m_points);
    }
    //return m_actives > 0;
}

bool Tilze::canAdd() const {
    return !m_squeeze && m_actions.empty();
}

bool Tilze::setRecord(const std::string& file) {
    std::ofstream out;
    out.open(file,  std::ofstream::out | std::ofstream::trunc);
    if(out.good()) {
        m_record = file;
        return true;
    }
    return false;
}

void Tilze::record(int current_number, int stripe) {
    if(!m_record.empty()) {
        std::ofstream out;
        out.open(m_record,  std::ofstream::out | std::ofstream::app);
        out << current_number << " " << stripe << std::endl;
    }
}

