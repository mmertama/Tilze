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
public:
    using CubePtr = std::shared_ptr<Cube>;
    enum class Animation {Move, Fade};
    Game(GameObserver& obs);
    ~Game();
    void animate(const CubePtr& cube, Animation anim_type);
    void setPoints(int points);
    void setGameOver(int points);
    void setNumber(int number);
    void run();
    void drawOnce() override;
    void drawStart() override;
    void drawEnd() override;
    void add(const CubePtr& ptr, int next_value);
    void setPostAnimation(const std::function<void ()>& finished);
    bool isActive() const;
    bool isGameOver() const;
private:
    void resize();
    void drawFrame();
private:
    GameObserver& m_obs;
    View m_view;
    Animator m_animator;
    std::unique_ptr<Gempyre::CanvasElement> m_canvas;
    bool m_gameOver = false;
    bool m_onRedraw = false;
    std::optional<int> m_selected;
};
