#include <iostream>
#include <cassert>
#include <chrono>
#include <random>

#include "tilze.h"
#include "cube.h"
#include "view.h"
#include "game.h"
#include "autoplay.h"

namespace Gempyre {
    class FrameComposer;
}

static int get2Pow(int max) {
    std::default_random_engine eng(std::random_device {}());
    std::uniform_int_distribution<int> dis(1, max); //not working with old mingw
    const auto exp = dis(eng);
    return static_cast<int>(std::pow(2, exp));
    }

void draw(Gempyre::FrameComposer& fc, int w, int h, const Tilze& tilze) {
    for(const auto& c : tilze) {
        if(!c->isAnimated())
            c->draw(fc, w, h);
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

    GameFunctions functions = {
        ForwardFunction<void, int, int>(),
        ForwardFunction<int, int>(),
        ForwardFunction<void>(),
        ForwardFunction<void, Gempyre::FrameComposer&, int, int>()
    };

    Game game(functions);

    Tilze tilze(game);
    AutoPlay auto_play(game, tilze);
    View view;

    functions.select = [&](int stripe) {
        const int value = get2Pow(6);
        tilze.select(stripe, value);
        auto_play.add(stripe, value);
        return value;

    };

    functions.resize = [&](int width, int height) {
       for(auto it = tilze.begin() ; it != tilze.end(); ++it) {
           const auto xpos = view.stripePos(tilze.stripe(it));
           const auto ypos = tilze.level(it) * (height / RowCount);
           (*it)->repos(xpos, ypos);
       }
    };

    functions.reset = [&]() {
        tilze.clear();
    };

    functions.draw = [&](Gempyre::FrameComposer& fc, int width, int height) {
        ::draw(fc, width, height, tilze);
    };

    if(argc > 1)
        auto_play.play(argv[1]);

    game.run();

    return 0;
}
