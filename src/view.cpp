#include "view.h"
#include <gempyre_graphics.h>

void View::draw(Gempyre::FrameComposer& fc, std::optional<int> selected) const  {
     fc.fillRect({0, 0, width(), height()});
    auto p = left();
    const std::string red{"magenta"};
    const std::string blue{"blue"};
    for(auto i = 0; i < StripeCount; i++) {
        const std::string color = selected && selected.value() == i ? red : blue;
        fc.fillStyle(color);
        fc.fillRect({
            p + MarginPx,
            0,
            stripeInWidth(),
            height()});
        p += stripeOutWidth();
    }
}
