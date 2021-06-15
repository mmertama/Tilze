#ifndef GAMEOBSERVER_H
#define GAMEOBSERVER_H

#include <memory>
#include <optional>

class Cube;
class View;

namespace Gempyre {
    class FrameComposer;
}

class GameObserver {
public:
    using CubePtr = std::shared_ptr<Cube>;
    virtual void draw(Gempyre::FrameComposer& fc) = 0;
    virtual void resize(const View& view) = 0;
    virtual std::optional<std::tuple<CubePtr, int>> select(int stripe) = 0;
    virtual void reset() = 0;
    virtual std::optional<std::tuple<int, int>> position(const CubePtr& ptr) const = 0;
};

#endif // GAMEOBSERVER_H
