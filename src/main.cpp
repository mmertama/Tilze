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
    Cube(int value, int stripe, int id) : m_value(value), m_data(stripe << 16 | id) {}
    void draw(FrameComposer& fc, int w, int h) const { 
        roundRect(fc, {m_x, m_y, w, h}, "cyan", 5);
        fc.fillStyle("black");
        fc.font("bold 24px arial");
        fc.textBaseline("middle");
        fc.textAlign("center");
        fc.fillText(std::to_string(m_value), m_x + w / 2, m_y + h / 2);
    }
    int stripe() const {
        return m_data >> 16;
    }
    bool operator==(const Cube& other) const {return m_data == other.m_data;}
    int value() const {return m_value;}
    int id() const {return m_data;}
    static int stripeFromId(int data) {return data >> 16;}
private:
    int m_value;
    int m_data;
};

class Table {
public:
    template<class T, class TT>
    struct Iterator  {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = typename T::difference_type;
        using value_type        = typename T::value_type;
        using pointer           = typename T::pointer;
        using reference         = typename T::reference;
        reference operator*() const {return m_it.operator*();}
        pointer operator->() {return m_it.operator->();}
        Iterator& operator++() {
            m_it++;
            while(m_it == m_it_end) {
                ++m_container_it;
                if(m_container_it == m_container_it_end)
                    break;
                m_it = m_container_it->begin();
                m_it_end = m_container_it->end();
            }
            return *this;
            }  
        Iterator operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
            }
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_it == b.m_it; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_it != b.m_it; };
        Iterator(T i, T e, TT c, TT ce) : 
        m_it(i), m_it_end(e), m_container_it(c), m_container_it_end(ce) {}
    private:
            T m_it;
            T m_it_end;
            TT m_container_it;
            TT m_container_it_end;
        };
public:
    using const_data_iterator = std::vector<Cube>::const_iterator;
    using const_data_iterator_iterator = std::vector<std::vector<Cube>>::const_iterator;
    using data_iterator = std::vector<Cube>::iterator;
    using data_iterator_iterator = std::vector<std::vector<Cube>>::iterator;
    Table() : m_cubes(StripeCount) {}
    Cube* add(int value, int stripe) {
        if(m_cubes[stripe].size() >= RowCount)
            return nullptr;
        m_cubes[stripe].emplace_back(value, stripe, ++m_ids);
        return &(m_cubes[stripe].back());
    }
    
    int level(int stripe) const {return m_cubes[stripe].size();}
    
    int level(const Cube& cube) const {
        const auto s = m_cubes[cube.stripe()];
        for(auto it = s.begin(); it != s.end(); ++it)
            if(cube == *it)
                return std::distance(s.begin(), it);
        assert(false);
        return -1;
        }

    std::vector<int> sisters(const Cube& cube) const {
        const auto s = cube.stripe();
        const auto l = level(cube);
        std::vector<int> sister_vec;
        sister_vec.reserve(5);
        if(s > 0 && level(s - 1) > l && m_cubes[s - 1][l].value() == cube.value())
            sister_vec.push_back(m_cubes[s - 1][l].id());
        if(s < m_cubes.size() - 1 && level(s + 1) > l && m_cubes[s + 1][l].value() == cube.value())
            sister_vec.push_back(m_cubes[s + 1][l].id());
        if(l > 1 && m_cubes[s][l - 1].value() == cube.value())
            sister_vec.push_back(m_cubes[s][l - 1].id());
        if(l < m_cubes[s].size() - 1 && m_cubes[s][l + 1].value() == cube.value())
            sister_vec.push_back(m_cubes[s][l + 1].id());
        if(sister_vec.size() > 0)
            sister_vec.push_back(cube.id());      
        return sister_vec;
    }

    void remove(int id) {
        const auto stripe = Cube::stripeFromId(id);
        auto s = m_cubes.at(stripe);
        for(auto it = s.begin(); it != s.end(); ++it)
            if(id == it->id()) {
                std::remove(s.begin(), s.end(), *it);
                std::cout << "erased:" << it->id() << " -> " << it->value() << std::endl;
                return;
            }
        assert(false);
    }

    template<class T, class TT>
    Iterator<T, TT> begin() {
        for(auto& vec : m_cubes) {
            if(!vec.empty())
                return Iterator{vec.begin(), vec.end(), m_cubes.begin(),  m_cubes.end()};
        }
        return end<T, TT>();
    }
    template<class T, class TT>
    Iterator<T, TT> end() {
        return Iterator{m_cubes.back().end(), m_cubes.back().end(), m_cubes.end(), m_cubes.end()};
    }
    template<class T, class TT>
    Iterator<T, TT> begin() const {
        for(auto& vec : m_cubes) {
            if(!vec.empty())
                return Iterator{vec.begin(), vec.end(), m_cubes.begin(),  m_cubes.end()};
        }
        return end<T, TT>();
    }
    template<class T, class TT>
    Iterator<T, TT> end() const {
        return Iterator{m_cubes.back().end(), m_cubes.back().end(), m_cubes.end(), m_cubes.end()};
    }
    auto begin() const { return begin<const_data_iterator, const_data_iterator_iterator>(); }
    auto end() const { return end<const_data_iterator, const_data_iterator_iterator>(); }
    auto begin() {return begin<data_iterator, data_iterator_iterator>(); }
    auto end() { return end<data_iterator, data_iterator_iterator>(); }
private:
    std::vector<std::vector<Cube>> m_cubes;
    int m_ids = 0;
};

void drawUi(CanvasElement& canvas, int width, int height, int selected, const Table& cubes) {
    FrameComposer fc;
    fc.fillStyle("black");
    fc.fillRect({0, 0, width, height});
    const Stripes stripes(width);
    auto p = stripes.start();
    const std::string red{"magenta"};
    const std::string blue{"blue"}; 
    for(auto i = 0; i < StripeCount; i++) {
        const std::string color = selected == i ? red : blue;
        fc.fillStyle(color);
        fc.fillRect({
            p + MarginPx,
            0,
            stripes.stripeWidth(),
            height});
        p += stripes.stripeWidth() + MarginPx;
    }
    const auto h = height / RowCount;
    const auto w = stripes.stripeWidth();
    for(const auto& c : cubes) {
        c.draw(fc, w, h);
    }
    canvas.draw(fc);
}

static int get2Pow(int up_to) {
    std::default_random_engine eng(std::random_device {}());
    std::uniform_int_distribution<int> dis(1, up_to);
    return std::pow(2, dis(eng)); 
    }

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    Ui ui(Ui_resourceh, "/ui.html");
    CanvasElement canvas(ui, "canvas");
    Element number(ui, "number");
    auto width = 0;
    auto height = 0;
    auto selected_stripe = -1;
    Table cubes;
    int max_pow = 6;
    auto current_number = 2;
    Animator<Cube> animator(&ui);

    
    const auto redraw = [&]() {
        drawUi(canvas, width, height, selected_stripe, cubes);
    };

        const auto add_cube = [&](int number, int stripe, int start_y) {
        const auto ani = cubes.add(number, stripe);
        if(ani) {
            const Stripes stripes(width);
            const auto xpos = stripes.stripePos(stripe);
            //-1 as this one was added
            const auto ypos = (cubes.level(stripe) - 1) * (height / RowCount);
            const auto period = (SlideSpeed * (height - ypos) / height);
            ani->animate(xpos, start_y, xpos, ypos, period);
            animator.addAnimation(ani);
        }
    };


        const auto merge = [&](const Cube& cube) {
        const auto sisters = cubes.sisters(cube);
        if(sisters.size() == 0)
            return;

        const auto stripe = cube.stripe();
        const auto ypos = (cubes.level(selected_stripe) - 1) * (height / RowCount);
        const auto value = cube.value();

        for(const auto id : sisters)
            cubes.remove(id);

        add_cube(value + value, stripe, ypos);    
    };

    const auto do_resized = [&]() {
        const auto rect = ui.root().rect();
        assert(rect);
        const auto dash_rect = Element(ui, "dash").rect();
        assert(dash_rect);
        width = rect->width;
        height = rect->height - dash_rect->height;
        canvas.setAttribute("width", std::to_string(width));
        canvas.setAttribute("height", std::to_string(height));
        animator.setSize(width, height);
        const Stripes stripes(width);
        for(Cube& c : cubes) {
            const auto xpos = stripes.stripePos(c.stripe());
            const auto ypos = cubes.level(c) * (height / RowCount);
            c.repos(xpos, ypos);
        }
        redraw();
    };

    ui.root().subscribe("resize", [&do_resized](auto) {
        do_resized();
    }, {}, 200ms);

    ui.onOpen([&do_resized] {
        do_resized();
    });

    canvas.subscribe("click", [&](const auto& ev) {
        if(animator.isActive())
            return; // Im too slow :-C
        const Stripes stripes(width);
        const auto x = GempyreUtils::to<int>(ev.properties.at("clientX"));
        selected_stripe = stripes.stripeAt(x);
        add_cube(current_number, selected_stripe, height);
        drawUi(canvas, width, height, selected_stripe, cubes);
        current_number = get2Pow(max_pow);
        number.setHTML(std::to_string(current_number));
    }, {"clientX", "clientY"});

    animator.setRedraw(redraw);
    animator.setFinished([&merge] (auto c) {merge(*c);});
    ui.run();
    return 0;
}
