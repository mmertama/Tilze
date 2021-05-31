#pragma once

#include "animator.h"
#include <cassert>

namespace Gempyre {
    class FrameComposer;
}

class Cube;

class Cube : public Animated {
public:

    Cube(int value) : m_value(value) {}
    void draw(Gempyre::FrameComposer& fc) const;
    void kill() {
        assert(m_alive);
        m_alive = false;
    }
    bool isAlive() const {
        return m_alive;
    }
    int value() const {return m_value;}
    void setValue(int value) {m_value = value;}
private:
    int m_value;
    bool m_alive = true;
};

