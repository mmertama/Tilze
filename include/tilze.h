#pragma once

#include <optional>
#include <functional>
#include <deque>
#include <string>
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
    bool canAdd() const;
    void clear();
    std::optional<CubePtr> select(int stripe, int value);
    std::optional<int> selected() const;
    auto begin() const {return m_cubes.begin();}
    auto end() const {return m_cubes.end();}
    template<typename IT>
    int stripe(const IT& it) const { return m_cubes.column(it);}
    template<typename IT>
    int level(const IT& it) const { return m_cubes.row(it);}
    void setHistory(int stripe, int value);
    std::optional<std::tuple<int, int>> position(const CubePtr& ptr);
    bool setRecord(const std::string& file);
 private:
    std::optional<CubePtr> addCube(int number, int stripe);
    void merge(const CubePtr& cube);
    void squeeze();
    void record(int current, int stripe);
private:
    Game& m_game;
    std::optional<int> m_selected_stripe{std::nullopt};
    CubeTable m_cubes;
    int m_current_number{2};
    std::optional<std::tuple<int, int>> m_history;
    int m_points{0};
    std::deque<std::function<void ()>> m_actions;
    bool m_squeeze{false};
    std::string m_record;
};

