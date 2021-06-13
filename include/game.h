#pragma once

#include <functional>
#include <optional>
#include "gameenv.h"
#include "view.h"
#include "animator.h"

class Cube;
class GameObserver;

namespace Gempyre {
    class FrameComposer;
}

class Game : public GameEnv {
    using CubePtr = std::shared_ptr<Cube>;
    using CubeInfo = std::optional<std::tuple<CubePtr, int, int, int>>;
public:
    Game(GameObserver& obs);
    ~Game();
    void animate(const CubePtr&, int, int);
    void setPoints(int points);
    void setGameOver(int points);
    void setNumber(int number);
    void run();
    void draw() override;
    void requestDraw();
    void add(CubeInfo ptr);
    void setPostAnimation(const std::function<void ()>& finished);
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
