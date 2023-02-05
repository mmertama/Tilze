#include "gameenv.h"
#include "ui_resource.h"
#include <gempyre.h>
#include <gempyre_graphics.h>
#include <gempyre_utils.h>

GameEnv::GameEnv(int argc, char* argv[]) :
    m_ui{std::make_unique<Gempyre::Ui>(Ui_resourceh, "/ui.html")} {
}

int GameEnv::startPeriodic(const std::chrono::milliseconds &period, const std::function<void ()>& f) {
    return m_ui->start_periodic(period, f);
}

void GameEnv::after(const std::chrono::milliseconds &period, const std::function<void ()>& f) {
    m_ui->after(period, f);
}

void GameEnv::stopPeriodic(int perdiodic) {
    m_ui->cancel_timer(perdiodic);
}

GameEnv::~GameEnv() {}
