#include "animator.h"
#include <cassert>

/*
int Tilze::time(int from, int to) const {
    return SlideSpeed * std::abs(from - to) / m_height;
}

int Tilze::yPos(int level) const {
    return level * (m_height / RowCount);
}
*/

void Animated::animate(int ex, int ey, const std::chrono::milliseconds& speed, const std::function<void ()>& finished) {
    assert(finished);
    mFinished = finished;

    m_end_x = ex;
    m_end_y = ey;

    const auto ax = std::abs(m_x - m_end_x);
    const auto ay = std::abs(m_y - m_end_y);

    const auto period  = speed * std::max(ax, ay);

    const auto tics = period.count() / static_cast<double>(TimerPeriod.count());
    if(tics > 0) {
        auto dx = ax / tics;
        auto dy = ay / tics;
        m_dx = dx > 0 ? dx * ax / (m_end_x - m_x) : 0;
        m_dy = dy > 0 ? dy * ax / (m_end_y - m_y) : 0;
        } else {
            m_x = m_end_x;
            m_y = m_end_y;
        }
}

void Animated::finish() {
    assert(mFinished);
    auto f = mFinished;
    mFinished = nullptr;
    f();
}

void Animator::addAnimation(const value_type& ani) {
    m_animates.emplace_back(ani);
    if(!isActive()) {
        m_timerId = m_env.startPeriodic(TimerPeriod, [this]() {
            bool isRemoved = false;
            for(auto& ani : m_animates) {
                if(!ani->inc() && ani->isAnimated()) {
                    ani->finish();
                    isRemoved = true;
                    }
                }

            if(isRemoved) {
                m_animates.erase(
                            std::remove_if(m_animates.begin(), m_animates.end(), [](const auto& c) {
                    return !c->isAnimated();
                }),
                            m_animates.end());
            }

            if(isActive() && m_animates.empty()) {
                m_env.stopPeriodic(m_timerId);
                m_timerId = 0;
                }

            m_env.draw();
        });
    }
}


