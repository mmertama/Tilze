#include <iostream>
#include <cassert>
#include <chrono>
#include <random>

#include "tilze.h"
#include "animator.h"
#include "cube.h"
#include "view.h"
#include "game.h"
#include "autoplay.h"

constexpr auto SlideSpeed = TimerPeriod * 40;

namespace Gempyre {
    class FrameComposer;
}

static int get2Pow(int max) {
    std::default_random_engine eng(std::random_device {}());
    std::uniform_int_distribution<int> dis(1, max); //not working with old mingw
    const auto exp = dis(eng);
    return static_cast<int>(std::pow(2, exp));
    }

void draw(Gempyre::FrameComposer& fc, const View& view, const Tilze& tilze, const Animator& animator)  {

    view.draw(fc, tilze.selected());
    const auto h = view.cubeHeight();
    const auto w = view.stripeInWidth();

    for(const auto& c : tilze) {
        if(!c->isAnimated())
            c->draw(fc, w, h);
    }

    for(const auto& a : animator) {
        a->draw(fc, view.width(), view.height());
    }
}

// is there ready make way to do this
template <typename R, typename ...Args>
class ForwardFunction {
public:
    using Binded = std::function <R (Args...)>;
    ForwardFunction() : m_f(std::make_shared<Binded>(nullptr)) {}
    ForwardFunction(const ForwardFunction& other) = default;
    ForwardFunction& operator=(const ForwardFunction& other) = default;
    ForwardFunction& operator=(Binded&& f) {*m_f = std::move(f);return *this;}
    R operator()(Args... args) {return (*m_f)(args...);}
private:
    std::shared_ptr<Binded> m_f;
};

int main(int argc, char** argv) {
    View view;

    ForwardFunction<int, int, int> select;
    ForwardFunction<void, int, int> resize;
    ForwardFunction<void> reset;

    Game game(select, resize, reset);
    Animator animator(game);
    Tilze tilze(game);
    AutoPlay auto_play(game, tilze);

    select = [&](int x, int y) {
        (void) y;
        const int value = get2Pow(6);
        const auto stripe = view.stripeAt(x);
        tilze.select(stripe, value);
        auto_play.add(stripe, value);
        return value;

    };

    resize = [&](int width, int height) {
       view.set(width, height);
       for(auto it = tilze.begin() ; it != tilze.end(); ++it) {
           const auto xpos = view.stripePos(tilze.stripe(it));
           const auto ypos = tilze.level(it) * (height / RowCount);
           (*it)->repos(xpos, ypos);
       }
    };

    reset = [&]() {
        tilze.clear();
    };

    const auto animate = [&](const auto& cube, int stripe ,int level, auto finisher){
        const auto ypos = view.cubeHeight() * level;
        const auto x = view.stripePos(stripe);
        cube->animate(cube->x(), cube->y(), x, ypos,  SlideSpeed, finisher);
        animator.addAnimation(cube);
    };

    if(argc > 1)
        auto_play.play(argv[1]);

    const auto draw = [&](Gempyre::FrameComposer& fc) {
        ::draw(fc, view, tilze, animator);
    };

    return 0;
}
