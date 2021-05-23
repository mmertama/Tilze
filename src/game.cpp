#include "tilze.h"
#include "view.h"
#include "animator.h"
#include "cube.h"
#include "game.h"

#include <gempyre_graphics.h>
#include <gempyre.h>
#include <gempyre_utils.h>

using namespace Gempyre;

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

Game::Game(const Select& select, const Resize& resize, const Reset& reset)  :
    m_select(select),
    m_resize(resize),
    m_reset(reset),
    m_canvas(std::make_unique<Gempyre::CanvasElement>(*m_ui, "canvas")){
    Element(*m_ui, "restart").subscribe("click", [this](auto) {
         Element(*m_ui, "game_over_win").setAttribute("style", "visibility:hidden");
         m_gameOver = false;
         m_reset();
    });

    m_canvas->subscribe("click", [this](const auto& ev) {
        if(m_gameOver)
            return;
        const auto x = GempyreUtils::to<int>(ev.properties.at("clientX"));
        auto value = m_select(x, 0);
        setNumber(value);
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

    m_resize(width, height);

    draw();
};

void Game::draw()  {
    FrameComposer fc;
    fc.fillStyle("black");
    m_draw(fc);
    m_canvas->draw(fc);
}

void Game::run() {
    m_ui->run();
}
