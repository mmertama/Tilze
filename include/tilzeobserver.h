#ifndef TILZEOBSERVER_H
#define TILZEOBSERVER_H

#include "gameobserver.h"

class Tilze;
class AutoPlay;
class View;

namespace Gempyre {
    class FrameComposer;
}

class TilzeObserver : public GameObserver {
public:
    TilzeObserver();
    TilzeObserver& operator=(Tilze& tilze);
    TilzeObserver& operator=(AutoPlay& play);
    void draw(Gempyre::FrameComposer& fc) override;
    void resize(const View& view) override;
    std::optional<std::tuple<CubePtr, int>> select(int stripe) override;
    void reset() override;
    std::optional<std::tuple<int, int>> position(const CubePtr& ptr) const override;
private:
    Tilze* m_tilze = nullptr;
    AutoPlay* m_play = nullptr;
    int m_nextValue;
};


#endif // TILZEOBSERVER_H
