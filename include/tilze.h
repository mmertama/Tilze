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
    using CubePos = std::optional<std::tuple<CubePtr, int, int>>;
    using CubeTable = Table<Cube, StripeCount, RowCount>;
public:
    Tilze(Game& game);
    bool canAdd() const;
    void clear();
    CubePos select(int stripe, int value);
    std::optional<int> selected() const;
    auto begin() const {return m_cubes.begin();}
    auto end() const {return m_cubes.end();}
    template<typename IT>
    int stripe(const IT& it) const { return m_cubes.column(it);}
    template<typename IT>
    int level(const IT& it) const { return m_cubes.row(it);}
    void setHistory(int stripe, int value);
 private:
    CubePos addCube(int number, int stripe, int start_y);
    void merge(const CubePtr& cube, int stripe, int level);
    void squeeze();
private:
    Game& m_game;
    std::optional<int> m_selected_stripe = std::nullopt;
    CubeTable m_cubes;
    int m_current_number = 2;
    std::optional<std::tuple<int, int>> m_history;
    int m_points = 0;
    bool m_active = false;
};
