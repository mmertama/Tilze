#include "tilze.h"
#include "view.h"
#include "animator.h"
#include "cube.h"
#include "game.h"
#include "gameobserver.h"

#include <gempyre_graphics.h>
#include <gempyre.h>
#include <gempyre_utils.h>

using namespace Gempyre;
constexpr auto SlideSpeed = TimerPeriod * 40;
constexpr auto FadeSpeed = TimerPeriod * 10;

void Game::setPoints(int points) {
    Element(*m_ui, "points").setHTML(std::to_string(points));
}

void Game::setGameOver(int points) {
    m_gameOver = true;
        Element(*m_ui, "game_over_win").setAttribute("style", "visibility:visible");
        Element(*m_ui, "game_over_points").setHTML(std::to_string(points));
}

void Game::setNumber(int value) {
    Element(*m_ui, "number").setHTML(std::to_string(value));
}

Game::~Game() {}

void Game::add(const CubePtr& cube, int next) {
    const auto p =  m_obs.position(cube);
    assert(p);
    const auto& [stripe, level] = *p;
    *m_selected = stripe; //autoplay can make them diffrent
    const auto h = m_view.height() / RowCount;
    const auto w = m_view.stripeInWidth();
    const auto x_pos = m_view.stripePos(stripe);
    const auto y_pos = m_view.height();
    cube->setExtents(x_pos, y_pos, w, h);
    cube->animate(x_pos, level * cube->height(), 1s);
    m_animator.addAnimation(cube);
    setNumber(next);
}

Game::Game(GameObserver& obs)  :
    m_obs(obs),
    m_animator(*this),
    m_canvas(std::make_unique<Gempyre::CanvasElement>(*m_ui, "canvas")) {
    Gempyre::setDebug(Gempyre::DebugLevel::Info);
    Element(*m_ui, "restart").subscribe("click", [this](auto) {
         Element(*m_ui, "game_over_win").setAttribute("style", "visibility:hidden");
         m_gameOver = false;
         m_obs.reset();
    });

    m_canvas->subscribe("click", [this](const auto& ev) {
        if(m_gameOver || m_animator.isActive())
            return;
        const auto x = GempyreUtils::to<int>(ev.properties.at("clientX"));
        m_selected = m_view.stripeAt(x);
        const auto ptr = m_obs.select(*m_selected);
        if(ptr) {
           add(std::get<CubePtr>(*ptr), std::get<int>(*ptr));
        }
    }, {"clientX", "clientY"}, 200ms);


    m_ui->root().subscribe("resize", [this](auto) {
       this->resize();
    }, {}, 200ms);

    m_ui->onOpen([this] {
        this->resize();
    });
}

void Game::resize() {
    const auto rect = m_ui->root().rect();
    assert(rect);
    const auto dash_rect = Element(*m_ui, "dash").rect();
    assert(dash_rect);
    const auto width = rect->width;
    const auto height = rect->height - dash_rect->height;
    m_canvas->setAttribute("width", std::to_string(width));
    m_canvas->setAttribute("height", std::to_string(height));

    m_view.set(width, height);
    m_obs.resize(m_view);

    draw();
};

void Game::draw()  {
    FrameComposer fc;
    fc.fillStyle("black");
    m_view.draw(fc, m_selected);
    m_obs.draw(fc);
    for(const auto& a : m_animator) {
        a->draw(fc);
    }
    m_canvas->draw(fc);
}

void Game::animate(const CubePtr& cube, Animation animation_type) {
    switch (animation_type) {
        case Animation::Move: {
            const auto p =  m_obs.position(cube);
            assert(p);
            const auto& [stripe, level] = *p;
            const auto ypos = m_view.cubeHeight() * level;
            const auto x = m_view.stripePos(stripe);
            cube->animate(x, ypos,  SlideSpeed);
            }
            break;
        case Animation::Fade: {
            cube->animate(0.,  FadeSpeed);
            }
            break;
    }
    m_animator.addAnimation(cube);
}

void Game::setPostAnimation(const std::function<void ()>& finished) {
    m_animator.setPostAnimation(finished);
}

void Game::run() {
    m_ui->run();
}

void Game::requestDraw() {
    GempyreUtils::log(GempyreUtils::LogLevel::Info, "requestDraw", m_onRedraw);
    if(!m_onRedraw) {
        m_onRedraw = true;
        m_ui->startTimer(0ms, true, [this]() {
            GempyreUtils::log(GempyreUtils::LogLevel::Info, "requestDraw - act", m_animator.isActive());
            if(!m_animator.isActive()) {
                draw();
                m_onRedraw = false;
            }
        });
    }
}

