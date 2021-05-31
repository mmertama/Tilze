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
    void animate(int end_x, int end_y, const std::chrono::milliseconds& speed, const std::function<void ()>& finished);
    void finish();

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

    void setExtents(int x, int y, int w, int h) {
        m_x = x;
        m_y = y;
        m_width = w;
        m_height = h;
    }

    int x() const {return static_cast<int>(m_end_x);}
    int y() const {return static_cast<int>(m_end_y);}
    int width() const {return static_cast<int>(m_width);}
    int height() const {return static_cast<int>(m_height);}

    virtual void draw(Gempyre::FrameComposer& fc) const = 0;
protected:
    std::function<void()> mFinished;
    double m_x, m_y;
    double m_end_x, m_end_y;
    double m_dx = 0, m_dy = 0;
    double m_width, m_height;
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

