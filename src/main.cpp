#include <gempyre.h>
#include <gempyre_graphics.h>
#include <gempyre_utils.h>
#include "ui_resource.h"
#include <iostream>
#include <cassert>
#include <chrono>
#include <list>
#include <random>

#include "animator.h"
#include "sparseiterator.h"


using namespace Gempyre;
constexpr auto SlideSpeed = TimerPeriod * 20;

static void roundRect(FrameComposer& fc, const Element::Rect& rect, const std::string& color, int radius) {
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

class Cube : public Animated {
public:
    Cube(int value, int stripe) : m_value(value), m_stripe(stripe) {}
    void draw(FrameComposer& fc, int w, int h) const { 
        roundRect(fc, {m_x, m_y, w, h}, "cyan", 5);
        fc.fillStyle("black");
        fc.font("bold 24px arial");
        fc.textBaseline("middle");
        fc.textAlign("center");
        fc.fillText(std::to_string(m_value), m_x + w / 2, m_y + h / 2);
    }
    int stripe() const {
        return m_stripe;
    }
    int value() const {return m_value;}
    void setValue(int value) {m_value = value;}
private:
    int m_value;
    int m_stripe;
};

template <int ROWS, int STRIPES>
class Table {
private:
    using value_type = std::shared_ptr<Cube>;
    using ArrayType = std::array<value_type, ROWS * STRIPES>;
    using Array_const_iterator = typename ArrayType::const_iterator;
    using Array_iterator = typename ArrayType::iterator;
    using SparseIteratorType_const = SparseIterator<Array_const_iterator,
        const std::ptrdiff_t,
        const typename ArrayType::value_type,
        const typename ArrayType::value_type*,
        const typename ArrayType::value_type&>;
    using SparseIteratorType = SparseIterator<Array_iterator,
        std::ptrdiff_t,
        typename ArrayType::value_type,
        typename ArrayType::value_type*,
        typename ArrayType::value_type&>;
private:
    int pos(int stripe, int row) const {
        return ROWS * stripe + row;
    }
    auto at(int stripe, int level) const {
        return m_cubes[pos(stripe, level)];
    }
    auto take(int stripe, int level) {
        return std::move(m_cubes[pos(stripe, level)]);
    }
public:
    auto begin() const { return SparseIteratorType_const{m_cubes.begin(), m_cubes.end()}; }
    auto end() const { return SparseIteratorType_const{m_cubes.end(), m_cubes.end()}; }
    auto begin() { return SparseIteratorType{m_cubes.begin(), m_cubes.end()}; }
    auto end() {return SparseIteratorType{m_cubes.end(), m_cubes.end()}; }
    Table() {}
    value_type add(int value, int stripe) {
        const auto sz = size(stripe);
        if(sz >= ROWS)
            return nullptr;
        const auto p = pos(stripe, sz);
        assert(!m_cubes[p]);
        m_cubes[p] = std::make_shared<Cube>(value, stripe);
        return m_cubes[p];
    }

    value_type move(const Cube& cube, int row) {
        const auto p = pos(cube.stripe(), row);
        assert(!m_cubes[p]);
        m_cubes[p] = take(cube.stripe(), level(cube));
        return m_cubes[p];
    }

    int size(int stripe) const {
        const auto begin = pos(stripe, 0);
        const auto end = begin + ROWS;
        for(auto it = begin; it != end; ++it)
            if(!m_cubes[it])
                return it - begin;
        return ROWS;
    }

    int level(int stripe) const {return size(stripe);}
    
    int level(const Cube& cube) const {
        const auto begin = pos(cube.stripe(), 0);
        const auto end = begin + ROWS;
         for(auto it = begin; it != end; ++it)
            if(&cube == m_cubes[it].get())
                return it - begin;
        assert(false);
        return -1;
        }

    bool has(int stripe, int level) const {
        return at(stripe, level).operator bool();
    }

    std::vector<value_type> takeSisters(const Cube& cube) {
        const auto s = cube.stripe();
        const auto l = level(cube);
        std::vector<value_type> vec;
        vec.reserve(5);
        const auto push = [&vec, this](int s, int l) { vec.emplace_back(take(s, l));};
        if(s > 0 && at(s - 1, l) && at(s - 1, l)->value() == cube.value())
            push(s - 1, l);
        if(s + 1 < STRIPES && at(s + 1, l) && at(s + 1, l)->value() == cube.value())
            push(s + 1, l);
        if(l > 0 && at(s, l - 1)->value() == cube.value())
            push(s, l - 1);
        if(l + 1 < STRIPES && at(s, l + 1) && at(s, l + 1)->value() == cube.value())
            push(s, l + 1);   
        return vec;
    }

private:
        ArrayType m_cubes;
};

using CubeTable = Table<RowCount, StripeCount>;

class Tilze {
 private:
    auto time(int from, int to) const {
        return SlideSpeed * std::abs(from - to) / m_height;
    }   
    int get2Pow() const {
        std::default_random_engine eng(std::random_device {}());
        std::uniform_int_distribution<int> dis(1, m_max_pow);
        return std::pow(2, dis(eng)); 
        }
    int yPos(int level) const {
        return level * (m_height / RowCount);
    }
public:    
    Tilze(const CanvasElement& canvas) : 
        m_canvas(canvas),
        m_animator(Animator(&m_canvas.ui(), [this](){draw();})) {}

    void addCube(int number, int stripe, int start_y) {
        const auto ani = m_cubes.add(number, stripe);
        if(ani) {
            const Stripes stripes(m_width);
            const auto xpos = stripes.stripePos(stripe);
            //-1 as this one was added
            const auto ypos = yPos(m_cubes.level(stripe) - 1);
            const auto period = time(m_height, ypos);
            ani->animate(xpos, start_y, xpos, ypos, period, [ani, this]() {
                merge(*ani);
                });
            m_animator.addAnimation(ani);
        }
    }

    void draw() const {
        FrameComposer fc;
        fc.fillStyle("black");
        fc.fillRect({0, 0, m_width, m_height});
        const Stripes stripes(m_width);
        auto p = stripes.start();
        const std::string red{"magenta"};
        const std::string blue{"blue"}; 
        for(auto i = 0; i < StripeCount; i++) {
            const std::string color = m_selected_stripe == i ? red : blue;
            fc.fillStyle(color);  
            fc.fillRect({
                p + MarginPx,
                0,
                stripes.stripeWidth(),
                m_height});
            p += stripes.stripeWidth() + MarginPx;
        }
        const auto h = m_height / RowCount;
        const auto w = stripes.stripeWidth();
        for(const auto& c : m_cubes) {
            c->draw(fc, w, h);
        }
        m_canvas.draw(fc);
    }

    void merge(Cube& cube) {
        auto sisters = m_cubes.takeSisters(cube);
        if(sisters.size() == 0)
            return;
        const auto value = cube.value();
        cube.setValue(value + value);
        squeeze();
        draw();  
    }
 
    void squeeze() {
        for(auto& c : m_cubes) {
            const auto level = m_cubes.level(*c);
            auto next_level = level;
            while(next_level > 0 && !m_cubes.has(c->stripe(), next_level - 1)) {
                --next_level;
            }
            if(next_level < level) {
                auto ani = m_cubes.move(*c, level - 1);
                const auto ypos = yPos(level - 1);
                const auto period = time(ani->y(), ypos);
                ani->animate(ani->x(), ani->y(), ani->x(), ypos, period, [ani, *this]() mutable {
                    merge(*ani);
                });
                m_animator.addAnimation(ani);
            }
        }
    }

    void resize(int width, int height) {
        m_width = width;
        m_height = height;
        m_animator.setSize(width, height);
        const Stripes stripes(width);
        for(auto& c : m_cubes) {
            const auto xpos = stripes.stripePos(c->stripe());
            const auto ypos = m_cubes.level(*c) * (height / RowCount);
            c->repos(xpos, ypos);
        }
        draw();
    }

    int select(int x) {
        if(m_animator.isActive())
            return m_current_number; // Im too slow :-C
        const Stripes stripes(m_width);
        m_selected_stripe = stripes.stripeAt(x);
        addCube(m_current_number, m_selected_stripe, m_height);
        draw();
        m_current_number = get2Pow();
        return m_current_number;
    }

private:
    CanvasElement m_canvas;
    int m_width = 0;
    int m_height = 0;
    int m_selected_stripe = -1;
    CubeTable m_cubes;
    int m_max_pow = 6;
    int m_current_number = 2;
    Animator m_animator;
};

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
  //  Gempyre::setLogging(Gempyre::LogLevel::Debug);
    Ui ui(Ui_resourceh, "/ui.html");
    CanvasElement canvas(ui, "canvas");
    Element number(ui, "number");
    Tilze tilze(canvas);

    const auto redraw = [&tilze]() {
        tilze.draw();
    };

    const auto do_resized = [&]() {
        const auto rect = ui.root().rect();
        assert(rect);
        const auto dash_rect = Element(ui, "dash").rect();
        assert(dash_rect);
        const auto width = rect->width;
        const auto height = rect->height - dash_rect->height;
        canvas.setAttribute("width", std::to_string(width));
        canvas.setAttribute("height", std::to_string(height));
        tilze.resize(width, height);
    };

    ui.root().subscribe("resize", [&do_resized](auto) {
        do_resized();
    }, {}, 200ms);

    ui.onOpen([&do_resized] {
        do_resized();
    });

    canvas.subscribe("click", [&](const auto& ev) {
        const auto x = GempyreUtils::to<int>(ev.properties.at("clientX"));
        const auto current_number = tilze.select(x);
        number.setHTML(std::to_string(current_number));
    }, {"clientX", "clientY"}, 200ms);

    ui.run();
    return 0;
}
