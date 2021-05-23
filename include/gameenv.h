#pragma once

#include <functional>
#include <chrono>
#include <memory>

namespace Gempyre {
    class Ui;
    class CanvasElement;
}

class GameEnv {
public:
    GameEnv();
    ~GameEnv();
    void after(const std::chrono::milliseconds& f, const std::function<void()>&);
    int startPeriodic(const std::chrono::milliseconds& f, const std::function<void()>&);
    void stopPeriodic(int perdiodic);
    virtual void draw() = 0;
protected:
    std::unique_ptr<Gempyre::Ui> m_ui;
};
