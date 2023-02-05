#include "view.h"
#include <gempyre_graphics.h>

void View::draw(Gempyre::FrameComposer& fc, std::optional<int> selected) const  {
     fc.fill_rect({0, 0, width(), height()});
    auto p = left();
    const std::string red{"darkgreen"};
    const std::string blue{"darkred"};
    for(auto i = 0; i < StripeCount; i++) {
        const std::string color = selected && selected.value() == i ? red : blue;
        fc.fill_style(color);
        fc.fill_rect({
            p + MarginPx,
            0,
            stripeInWidth(),
            height()});
        p += stripeOutWidth();
    }
}
