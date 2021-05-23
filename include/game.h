#pragma once

#include <functional>
#include "gameenv.h"

class Cube;
class View;

namespace Gempyre {
    class FrameComposer;
}

class Game : public GameEnv {
    using CubePtr = std::shared_ptr<Cube>;
    using Resize = std::function<void (int width, int height)>;
    using Select = std::function<int (int x, int y)>;
    using Reset = std::function<void ()>;
public:
    Game(const Select& select, const Resize& resize, const Reset& reset);
    ~Game();
    void animate(const CubePtr&, int, int, const std::function<void()>&);
    void setPoints(int points);
    void setGameOver(int points);
    void setNumber(int number);
    void run();
    void draw() override;
private:
    void resize();
public:
    Select m_select;
    Resize m_resize;
    Reset m_reset;
    std::unique_ptr<Gempyre::CanvasElement> m_canvas;
    std::function<void (Gempyre::FrameComposer&)> m_draw;
    bool m_gameOver = false;
};
