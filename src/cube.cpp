#include "cube.h"
#include "gempyre_graphics.h"
#include "gempyre.h"

static void roundRect(Gempyre::FrameComposer& fc, const Gempyre::Element::Rect& rect, const std::string& color, int radius) {
  fc.beginPath();
  fc.fillStyle(color);
  fc.moveTo(rect.x + radius, rect.y);
  fc.lineTo(rect.x + rect.width - radius, rect.y);
  fc.quadraticCurveTo(rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + radius);
  fc.lineTo(rect.x + rect.width, rect.y + rect.height - radius);
  fc.quadraticCurveTo(rect.x + rect.width, rect.y + rect.height, rect.x + rect.width - radius, rect.y + rect.height);
  fc.lineTo(rect.x + radius, rect.y + rect.height);
  fc.quadraticCurveTo(rect.x, rect.y + rect.height, rect.x, rect.y + rect.height - radius);
  fc.lineTo(rect.x, rect.y + radius);
  fc.quadraticCurveTo(rect.x, rect.y, rect.x + radius, rect.y);
  fc.closePath();
  fc.fill();
}

void Cube::draw(Gempyre::FrameComposer& fc) const {
    roundRect(fc, {x(), y(), width(), height()}, "cyan", 5);
    fc.fillStyle("black");
    fc.font("bold 24px arial");
    fc.textBaseline("middle");
    fc.textAlign("center");
    fc.fillText(std::to_string(m_value), m_x + m_width / 2., m_y + m_height / 2.);
}
