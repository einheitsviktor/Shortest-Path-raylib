#pragma once

#include <functional>
#include <raylib.h>
#include <tuple>

enum class TileState {
    empty,
    obstacle,
    visited,
    start,
    goal, 
    path
};

enum class ButtonState {
    normal,
    mouse_hover,
    pressed
};

struct Tile {
    Tile() {};
    Tile(int y, int x, int recY, int recX, int recWidth, int recHeight) : y(y) , x(x) {
        rec.y = recY;
        rec.x = recX;
        rec.width = recWidth;
        rec.height = recHeight;
    }
    
    int y, x;
    Rectangle rec;
    TileState tileState = TileState::empty;
    ButtonState buttonState = ButtonState::normal;

    // Getter functions
    bool isEmpty()    const { return this->tileState == TileState::empty; }
    bool isObstacle() const { return this->tileState == TileState::obstacle; }
    bool isVisited()  const { return this->tileState == TileState::visited; }
    bool isStart()    const { return this->tileState == TileState::start; }
    bool isGoal()     const { return this->tileState == TileState::goal; }
    bool isPath()     const { return this->tileState == TileState::path; }

    bool buttonNormal()  const { return this->buttonState == ButtonState::normal; }
    bool buttonHover()   const { return this->buttonState == ButtonState::mouse_hover; }
    bool buttonPressed() const { return this->buttonState == ButtonState::pressed; }
    
    // Setter
    void setTileEmpty() { this->tileState = TileState::empty; }
    void setTileObstacle() { this->tileState = TileState::obstacle; }
    void setTileVisited() { this->tileState = TileState::visited; }
    void setTileStart() { this->tileState = TileState::start; }
    void setTileGoal() { this->tileState = TileState::goal; }
    void setTilePath() { this->tileState = TileState::path; }

    void setButtonNormal() { this->buttonState = ButtonState::normal; }
    void setButtonHover() { this->buttonState = ButtonState::mouse_hover; }
    void setButtonPressed() { this->buttonState = ButtonState::pressed; }
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
