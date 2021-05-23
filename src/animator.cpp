#include "animator.h"

/*
int Tilze::time(int from, int to) const {
    return SlideSpeed * std::abs(from - to) / m_height;
}

int Tilze::yPos(int level) const {
    return level * (m_height / RowCount);
}
*/

void Animated::animate(int sx, int sy, int ex, int ey, const std::chrono::milliseconds& speed, const std::function<void ()>& finished) {
    assert(finished);
    mFinished = finished;

    m_x = sx;
    m_y = sy;
    m_end_x = ex;
    m_end_y = ey;

    const auto period  = speed * std::max(std::abs(sx - ex), std::abs(sy - ey));

    const auto tics = period.count() / static_cast<double>(TimerPeriod.count());
    if(tics > 0) {
        auto dx = std::abs(ex - sx) / tics;
        auto dy = std::abs(ey - sy) / tics;
        m_dx = dx > 0 ? dx * std::abs(ex - sx) / (ex - sx) : 0;
        m_dy = dy > 0 ? dy * std::abs(ey - sy) / (ey - sy) : 0;
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


