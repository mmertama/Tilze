#pragma once

#include <functional>
#include <optional>
#include "gameenv.h"
#include "view.h"
#include "animator.h"

class Cube;

namespace Gempyre {
    class FrameComposer;
}

class GameObserver {
public:
    using CubePtr = std::shared_ptr<Cube>;
    using CubeInfo = std::optional<std::tuple<CubePtr, int, int, int>>;
    virtual void draw(Gempyre::FrameComposer& fc) = 0;
    virtual void resize(const View& view) = 0;
    virtual CubeInfo select(int stripe) = 0;
    virtual void reset() = 0;
};

class Game : public GameEnv {
public:
    Game(GameObserver& obs);
    ~Game();
    void animate(const GameObserver::CubePtr&, int, int);
    void setPoints(int points);
    void setGameOver(int points);
    void setNumber(int number);
    void run();
    void draw() override;
    void requestDraw();
    void add(GameObserver::CubeInfo ptr);
private:
    void resize();
private:
    GameObserver& m_obs;
    View m_view;
    Animator m_animator;
    std::unique_ptr<Gempyre::CanvasElement> m_canvas;
    bool m_gameOver = false;
    bool m_onRedraw = false;
    std::optional<int> m_selected;
};
