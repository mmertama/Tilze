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

bool Animated::inc() {
    if(m_state & MoveAnimation)  {
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
    }

    if(m_state & FadeAnimation) {
        m_opacity -= m_opacity_delta;
        if((m_opacity_delta > 0 && m_opacity < m_opacity_end) ||
           (m_opacity_delta < 0 && m_opacity > m_opacity_end)) {
            m_opacity = m_opacity_end;
            return false;
        }
    }
    return true;
}

void Animated::setExtents(int x, int y, int w, int h) {
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;
}

void Animated::animate(double opacity, const std::chrono::milliseconds& speed) {
     m_state |= FadeAnimation;
     const auto period  = speed * (1. - opacity);
     const auto dur = period / TimerPeriod.count();
     const auto tics = std::chrono::duration<double, std::milli>(dur).count();
     m_opacity_end = opacity;
     if(tics > 0) {
           m_opacity_delta = (1. - opacity) / tics;
     } else {
         m_opacity = opacity;
     }
}

void Animated::animate(int ex, int ey, const std::chrono::milliseconds& speed) {

    m_state |= MoveAnimation;

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
    if(m_state != None) {
        m_state = None;
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
                if(mFinished)
                    mFinished();
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


