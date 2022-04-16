#pragma once

#include <functional>
#include <raylib.h>
#include <tuple>

enum class TileState {
    none,
    empty,
    obstacle,
    // visited,
    start,
    goal, 
    // path
};

enum class ButtonState {
    normal,
    mouse_hover,
    pressed
};

struct Tile {
    Tile() {}
    
    Rectangle rec;
    TileState tileState = TileState::empty;
    ButtonState buttonState = ButtonState::normal;

    // Getter functions
    bool isEmpty()    const { return this->tileState == TileState::empty; }
    bool isObstacle() const { return this->tileState == TileState::obstacle; }
    // bool isVisited()  const { return this->tileState == TileState::visited; }
    bool isStart()    const { return this->tileState == TileState::start; }
    bool isGoal()     const { return this->tileState == TileState::goal; }
    // bool isPath()     const { return this->tileState == TileState::path; }
};

struct Coordinates {
    int x, y;
    friend bool operator==(const Coordinates& a, const Coordinates& b) {
        return a.x == b.x && a.y == b.y;
    }
    friend bool operator!=(const Coordinates& a, const Coordinates& b) {
        return !(a == b);
    }
    friend bool operator<(const Coordinates& a, const Coordinates& b) {
        return std::tie(a.x, a.y) < std::tie(b.x, b.y);
    }
};

// "custom specialization of std::hash can be injected in namespace std"
// Implement hash to put Coordinates into an unordered_set
namespace std {
template<>
struct hash<Coordinates> {
    std::size_t operator()(const Coordinates& id) const noexcept {
        return std::hash<int>()(id.x ^ (id.y << 4));
    }
};
} // namespace std
