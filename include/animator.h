#pragma once

#include <chrono>
#include <cmath>
#include <gempyre_graphics.h>
#include <gempyre.h>

constexpr auto TimerPeriod = 30ms;
constexpr auto StripeCount = 8;
constexpr auto RowCount = 8;
constexpr auto MarginPx = 10;

class Stripes {
public:
    Stripes(int width) :
     m_w(width - MarginPx),
     m_stripeWidth((m_w - (m_w % StripeCount)) / StripeCount),
     m_start((m_w - (m_stripeWidth * StripeCount)) / 2) {}
     int stripeWidth() const {return m_stripeWidth - MarginPx;}
     int start() const {return m_start;}
     int stripeAt(int xpos) const {
         return (xpos * StripeCount) / m_w;
     }
     int stripePos(int stripe) const {
         return (m_start + MarginPx) + stripe * m_stripeWidth;
     }
private:
    const int m_w;
    const int m_stripeWidth;
    const int m_start;
};

class Animated {
public:
    void animate(int sx, int sy, int ex, int ey, std::chrono::milliseconds ms) {
        m_x = sx;
        m_y = sy;
        m_end_x = ex;
        m_end_y = ey;
        const auto tics = ms.count() / TimerPeriod.count();
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

    bool inc() {
        m_x += m_dx;
        m_y += m_dy;
        if(
            (m_dx > 0 && m_x >= m_end_x) ||
            (m_dx < 0 && m_x <= m_end_x) ||
            (m_dy > 0 && m_y >= m_end_y) ||
            (m_dy < 0 && m_y <= m_end_y)) {
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

    virtual void draw(Gempyre::FrameComposer& fc, int width, int height) const = 0;
protected:
    template<class A> friend class Animator;
    int m_x, m_y;
    int m_end_x, m_end_y;
    int m_dx = 0, m_dy = 0;
};

template<class A>
class Animator {
public:
    Animator(Gempyre::Ui* ui) : m_ui(ui) {
    }
    void setSize(int width, int height) {
        m_width = width;
        m_height = height;
    }
    void setRedraw(const std::function<void ()>& redraw) {mRedraw = redraw;}
    void setFinished(const std::function<void (A*)>& finished) {mFinished = finished;}
    void addAnimation(A* ani) {
        m_animates.emplace_back(ani);
        if(!isActive()) {
            m_timerId = m_ui->startTimer(TimerPeriod, false, [this]() {
                const auto h = m_height / RowCount;
                Stripes stripes(m_width);
                const auto w = stripes.stripeWidth(); 
                std::vector<A*> to_remove;
                for(auto& ani : m_animates) {
                    if(!ani->inc()) {
                        to_remove.push_back(ani);      
                    }
                }
                if(mRedraw)
                    mRedraw();
                for(auto& c : to_remove) {
                    if(mFinished)
                        mFinished(c);
                    m_animates.remove(c);
                }
                 if(m_animates.empty()) {
                     m_ui->stopTimer(m_timerId);
                     m_timerId = 0;
                     }         
            });
        }
    }
    bool isActive() const {
        return m_timerId > 0;
    }
private:
    Gempyre::Ui* m_ui;
    std::function<void()> mRedraw = nullptr;
    std::function<void(A*)> mFinished = nullptr;
    std::list<A*> m_animates;
    int m_timerId = 0;
    int m_width = 0;
    int m_height = 0;
};

