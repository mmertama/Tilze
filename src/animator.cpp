#include "animator.h"
#include <cassert>

#include "gempyre_utils.h"

/*
int Tilze::time(int from, int to) const {
    return SlideSpeed * std::abs(from - to) / m_height;
}

int Tilze::yPos(int level) const {
    return level * (m_height / RowCount);
}
*/

bool Animated::inc() {
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

void Animated::setExtents(int x, int y, int w, int h) {
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;
}

void Animated::animate(int ex, int ey, const std::chrono::milliseconds& speed) {

    m_state |= Animation;

    m_end_x = ex;
    m_end_y = ey;

    const auto ax = std::abs(m_x - m_end_x);
    const auto ay = std::abs(m_y - m_end_y);

    const auto period  = speed * std::max(ax, ay);
    const auto dur = period / TimerPeriod.count();

    const auto tics = std::chrono::duration<double>(dur).count();
    if(tics > 0) {
        auto dx = ax / tics;
        auto dy = ay / tics;
        m_dx = dx > 0 ? dx * ax / (m_end_x - m_x) : 0;
        m_dy = dy > 0 ? dy * ay / (m_end_y - m_y) : 0;
        } else {
            m_x = m_end_x;
            m_y = m_end_y;
        }
}

void Animated::setPostAnimation(const std::function<void ()> &finished) {
    assert(!mFinished);
    mFinished = finished;
}

void Animated::finish() {
    if(m_state & Animation) {
        m_state &= ~Animation;
        if(mFinished)
            mFinished();
    }
}

void Animator::setPostAnimation(const std::function<void ()> &finished) {
    mFinished = finished;
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
                GempyreUtils::log(GempyreUtils::LogLevel::Info, "END-A");
                if(mFinished)
                    mFinished();
                GempyreUtils::log(GempyreUtils::LogLevel::Info, "END-B", m_animates.size());
            }
            //we dont know about mFinished side effects
            if(isActive() && m_animates.empty()) {
                m_env.stopPeriodic(m_timerId);
                m_timerId = 0;
            }

            m_env.draw();
        });
    }
}


