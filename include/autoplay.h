#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "game.h"

class Tilze;

class AutoPlay  {
public:
    AutoPlay(Game& env, Tilze& tilze);
    void play(const std::string& name);
    void add(int stripe, int value);
private:
    Game& m_env;
    Tilze& m_tilze;
    std::ofstream m_stream;
    std::vector<std::pair<int, int>> m_playVec;
    std::vector<std::pair<int, int>>::iterator m_it;
    int m_timer = 0;
};
