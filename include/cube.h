#pragma once

#include "animator.h"

namespace Gempyre {
    class FrameComposer;
}

class Cube : public Animated {
public:

    Cube(int value/*, int stripe, int level*/) : m_value(value)/*, m_stripe(stripe), m_level(level)*/ {}

    void draw(Gempyre::FrameComposer& fc, int w, int h) const override;

    void kill() {
        assert(m_alive);
        m_alive = false;
    }

    bool isAlive() const {
        return m_alive;
    }

   /* int stripe() const {
        return m_stripe;
    }

    int level() const {return m_level;}
*/
    int value() const {return m_value;}

 //   void move(int level) {m_level = level;}

    void setValue(int value) {m_value = value;}

private:
    int m_value;
  //  const int m_stripe;
  //  int m_level;
    bool m_alive = true;
};

