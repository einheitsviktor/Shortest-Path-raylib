#pragma once

#include "Tile.hpp"

#include <raylib.h>
#include <array>


// MARKER: 
constexpr int SCREEN_WIDTH  = 1360;
constexpr int SCREEN_HEIGHT = 830;

constexpr float TILE_WIDTH  = 25.0f;
constexpr float TILE_HEIGHT = 25.0f;

constexpr int MAX_TILES_X = 50; // Columns
constexpr int MAX_TILES_Y = 25; // Rows


enum class Algorithm {
    Bfs,
    Dijkstra,
    AStar
};

class GUI {
public:
    GUI();
    ~GUI();
    void RunLoop();
private:
    void ProcessInput();
    void GenerateOutput();
    void Clear();

    Algorithm algorithm;
    Vector2 mousePosition = { 0.0f, 0.0f };
    TileState originState = TileState::none;
    Tile* startPtr;
    Tile* goalPtr;
    bool startButtonDrag = false;
    bool goalButtonDrag = false;

    Tile clearButton;
    Tile searchButton;

    std::array<std::array<Tile, MAX_TILES_X>, MAX_TILES_Y> grid;
    // std::array<std::array<int, MAX_TILES_X>, MAX_TILES_Y> colorState;
};
