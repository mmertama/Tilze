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

using CubePtr = std::shared_ptr<Cube>;

struct GameFunctions {
    using Resize = std::function<void (int width, int height)>;
    using Select = std::function<int (int stripe)>;
    using Reset = std::function<void ()>;
    using Draw = std::function<void (Gempyre::FrameComposer& fc, int , int)>;
    Resize resize;
    Select select;
    Reset reset;
    Draw draw;
};

class Game : public GameEnv {
public:
    Game(const GameFunctions& f);
    ~Game();
    void animate(const CubePtr&, int, int, const std::function<void()>&);
    void setPoints(int points);
    void setGameOver(int points);
    void setNumber(int number);
    void run();
    void draw() override;
    void requestDraw();
private:
    void resize();
private:
    GameFunctions m_f;
    View m_view;
    Animator m_animator;
    std::unique_ptr<Gempyre::CanvasElement> m_canvas;
    bool m_gameOver = false;
    bool m_onRedraw = false;
    std::optional<int> m_selected;
};
