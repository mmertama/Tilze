#pragma once

#include <algorithm>
#include <optional>

namespace Gempyre {
    class FrameComposer;
}


constexpr auto MarginPx = 10;

class View {
public:
    void set(int width, int height) {
        m_width = width;
        m_height = height;
    }
    int height() const {return m_height;}
    int width() const {return m_width;}

    int viewWidth() const {
        return width() - MarginPx;
    }

    int stripeOutWidth() const {
        return (viewWidth() - (viewWidth() % StripeCount)) / StripeCount;
    }

    int left() const {
        return ((viewWidth() - (stripeOutWidth() * StripeCount)) / 2);
    }

    int stripeInWidth() const {
        return stripeOutWidth() - MarginPx;
    }

    int stripeAt(int xpos) const {
        return std::min((xpos * StripeCount) / viewWidth(), StripeCount - 1);
    }

    int stripePos(int stripe) const {
        return (left() + MarginPx) + stripe * stripeOutWidth();
    }

    int cubeHeight() const {
        return height() / RowCount;
    }

    void draw(Gempyre::FrameComposer& fc, std::optional<int> selected) const;

private:
    int m_width = 0;
    int m_height = 0;
};
