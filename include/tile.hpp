#pragma once

#include <raylib.h>

#include <string>

enum class TileState { kEmpty, kObstacle, kVisited, kStart, kGoal, kPath };
enum class ButtonState { kNormal, kMouse_hover, kPressed };

struct Tile {
    int y, x;
    Rectangle rec;
    TileState tile_state;
    ButtonState button_state;
    std::string text;
    int font_size;

    Tile() = default;
    Tile(int y, int x, int rec_y, int rec_x, int rec_width, int rec_height, std::string s = "") : y(y), x(x) {
        rec.y = rec_y;
        rec.x = rec_x;
        rec.width = rec_width;
        rec.height = rec_height;
        text = s;
        font_size = 24;
        tile_state = TileState::kEmpty;
        button_state = ButtonState::kNormal;
    }

    // Getter functions
    bool IsTileEmpty() const {
        return tile_state == TileState::kEmpty;
    }
    bool IsTileObstacle() const {
        return tile_state == TileState::kObstacle;
    }
    bool IsTileVisited() const {
        return tile_state == TileState::kVisited;
    }
    bool IsTileStart() const {
        return tile_state == TileState::kStart;
    }
    bool IsTileGoal() const {
        return tile_state == TileState::kGoal;
    }
    bool IsTilePath() const {
        return tile_state == TileState::kPath;
    }
    bool IsButtonNormal() const {
        return button_state == ButtonState::kNormal;
    }
    bool IsButtonHover() const {
        return button_state == ButtonState::kMouse_hover;
    }
    bool IsButtonPressed() const {
        return button_state == ButtonState::kPressed;
    }

    // Setter
    void SetTileEmpty() {
        tile_state = TileState::kEmpty;
    }
    void SetTileObstacle() {
        tile_state = TileState::kObstacle;
    }
    void SetTileVisited() {
        tile_state = TileState::kVisited;
    }
    void SetTileStart() {
        tile_state = TileState::kStart;
    }
    void SetTileGoal() {
        tile_state = TileState::kGoal;
    }
    void SetTilePath() {
        tile_state = TileState::kPath;
    }

    void SetButtonNormal() {
        button_state = ButtonState::kNormal;
    }
    void SetButtonHover() {
        button_state = ButtonState::kMouse_hover;
    }
    void SetButtonPressed() {
        button_state = ButtonState::kPressed;
    }
};
