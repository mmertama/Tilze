#pragma once

#include <optional>
#include <functional>
#include "table.h"

class Cube;
class Animator;
class Animated;
class Game;

class Tilze {
    using CubePtr = std::shared_ptr<Cube>;
    using CubeTable = Table<Cube, StripeCount, RowCount>;
public:
    Tilze(Game& game);
    void resize(int width, int height);
    bool canAdd() const;
    void clear();
    void select(int stripe, int value);
    std::optional<int> selected() const;
    auto begin() const {return m_cubes.begin();}
    auto end() const {return m_cubes.end();}
    template<typename IT>
    int stripe(const IT& it) const { return m_cubes.column(it);}
    template<typename IT>
    int level(const IT& it) const { return m_cubes.row(it);}
    void setHistory(int stripe, int value);
    int current() const;
    bool isActive() const;
 private:

    void addCube(int number, int stripe, int start_y);
    void merge(const CubePtr& cube, int stripe, int level);
    void squeeze();
    //void draw() const;
    void requestDraw();
private:
    Game& m_game;
    std::optional<int> m_selected_stripe = std::nullopt;
    CubeTable m_cubes;
    int m_current_number = 2;
    bool m_onRedraw = false;
    std::optional<std::tuple<int, int>> m_history;
    int m_points = 0;
    bool m_active = false;
};
