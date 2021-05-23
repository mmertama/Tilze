#pragma once

#include <chrono>
#include <cmath>
#include <list>
#include <functional>
#include <gameenv.h>

using namespace std::chrono_literals;

constexpr auto TimerPeriod = 30ms;

namespace Gempyre {
    class FrameComposer;
}

class Animated {
public:
    virtual ~Animated() = default;
    void animate(int sx, int sy, int ex, int ey, const std::chrono::milliseconds& speed, const std::function<void ()>& finished);
    void finish() ;

    bool isAnimated()  const {
        return mFinished != nullptr;
    }

    bool inc() {
        m_x += m_dx;
        m_y += m_dy;
        if(
            (m_dx > 0 && m_x >= m_end_x) ||
            (m_dx < 0 && m_x <= m_end_x) ||
            (m_dy > 0 && m_y >= m_end_y) ||
            (m_dy < 0 && m_y <= m_end_y) ||
            (m_dx == 0 && m_dy == 0)) {
            m_x = m_end_x;
            m_y = m_end_y;
            return false;
        }
        return true;    
    }

    void repos(int x, int y) {
        m_x = x;
        m_y = y;
    }

    int x() const {return static_cast<int>(m_end_x);}
    int y() const {return static_cast<int>(m_end_y);}

    virtual void draw(Gempyre::FrameComposer& fc, int width, int height) const = 0;
protected:
    std::function<void()> mFinished;
    double m_x, m_y;
    double m_end_x, m_end_y;
    double m_dx = 0, m_dy = 0;
};

class Animator {
public:
    using value_type = std::shared_ptr<Animated>;
    Animator(GameEnv& env) : m_env(env) {
    }

    void addAnimation(const value_type& ani);

    auto begin() const {return m_animates.begin();}
    auto end() const {return m_animates.end();}

    bool isActive() const {
        return m_timerId > 0;
    }
private:
    GameEnv& m_env;
    std::list<value_type> m_animates;
    int m_timerId = 0;
};

