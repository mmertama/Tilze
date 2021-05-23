#include "gameenv.h"
#include "ui_resource.h"
#include <gempyre.h>
#include <gempyre_graphics.h>
#include <gempyre_utils.h>

GameEnv::GameEnv() :
    m_ui(std::make_unique<Gempyre::Ui>(Ui_resourceh, "/ui.html")) {
}

int GameEnv::startPeriodic(const std::chrono::milliseconds &period, const std::function<void ()>& f) {
    return m_ui->startTimer(period, false, f);
}

void GameEnv::after(const std::chrono::milliseconds &period, const std::function<void ()>& f) {
    m_ui->startTimer(period, true, f);
}

void GameEnv::stopPeriodic(int perdiodic) {
    m_ui->stopTimer(perdiodic);
}

GameEnv::~GameEnv() {}
