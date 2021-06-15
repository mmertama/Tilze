#include "autoplay.h"
#include "tilze.h"
#include "cube.h"
#include <filesystem>
#include <gempyre_utils.h>
#include <chrono>
#include <gameobserver.h>


using namespace std::chrono_literals;

static std::string uniqName() {
    auto path = std::filesystem::temp_directory_path() / "tilze";
    int p = 0;
    auto name = path.string() + ".txt";
    while(GempyreUtils::fileExists(name)) {
        ++p;
        name = path.string() + std::to_string(p) + ".txt";
    }
    return name;
}

AutoPlay::AutoPlay(Game& env, Tilze& tilze) : m_env(env), m_tilze(tilze), m_stream(uniqName()) {
}


void AutoPlay::play(const std::string& name) {
    assert(!m_timer);
    auto f = GempyreUtils::slurp(name);
    f.erase(std::find(f.begin(), f.end(), '\0'), f.end());
    const auto ss = GempyreUtils::split<std::vector<std::string>>(f, '\n');
    for(const auto& v : ss) {
        const auto s = GempyreUtils::split<std::vector<std::string>>(v, ' ');
        m_playVec.push_back(std::make_pair(GempyreUtils::to<int>(s[0]), GempyreUtils::to<int>(s[1])));
    }

    m_it = m_playVec.begin();

    m_timer = m_env.startPeriodic(500ms, [this]() {
        //GempyreUtils::log(GempyreUtils::LogLevel::Info, "set", std::distance(m_it, m_playVec.end()));
            if(m_it == m_playVec.end()) {
                m_env.stopPeriodic(m_timer);
                m_timer = 0;
                return;
            }
            else if(!m_tilze.canAdd()) {
                //m_tilze.setHistory(m_it->second, m_it->first);
                return;
            }

            const auto c = m_tilze.select(m_it->second, m_it->first);
            m_env.add(*c, (*c)->value());
            ++m_it;
            const auto dd = std::distance(m_playVec.begin(), m_it);
            GempyreUtils::log(GempyreUtils::LogLevel::Info, "autoplay", dd);
            });
}

void AutoPlay::add(int stripe, int value) {
    m_stream << value << " " << stripe << std::endl;
    m_stream.flush();
}

