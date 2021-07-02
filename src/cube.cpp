#include "cube.h"
#include "gempyre_graphics.h"
#include "gempyre.h"
#include <unordered_map>

static void roundRect(Gempyre::FrameComposer& fc,
                      const Gempyre::Element::Rect& rect,
                      const Gempyre::Color::type& color,
                      int radius) {
  fc.beginPath();
  fc.fillStyle(Gempyre::Color::rgba(color));
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
    static const std::unordered_map<int, Gempyre::Color::type> colors = {
        {2,     0x0000FF},
        {4,     0x00FF00},
        {8,     0xFF0000},
        {16,    0xFFFF00},
        {32,    0xFF00FF},
        {64,    0x00FFFF},
        {128,   0x00AA88},
        {256,   0x0088AA},
        {512,   0x8800AA},
        {1024,  0x888800},
        {2048,  0x880088},
        {4096,  0x008888},
        {8192,  0x44FF44},
        {16384, 0xFF4488},
        {32768, 0x884444}
    };
    const auto base_color = colors.at(value() & 0x7FFFF); //to prevent instant gameover on 65536
    auto color = Gempyre::Color::rgba(Gempyre::Color::r(base_color),
                                      Gempyre::Color::g(base_color),
                                      Gempyre::Color::b(base_color), static_cast<Gempyre::Color::type>(255. *  opacity()));
    roundRect(fc, {x(), y(), width(), height()}, color, 5);
    fc.fillStyle("black");
    fc.font("bold 24px arial");
    fc.textBaseline("middle");
    fc.textAlign("center");
    fc.fillText(std::to_string(m_value), m_x + m_width / 2., m_y + m_height / 2.);
}
