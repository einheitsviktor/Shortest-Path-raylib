#pragma once

#include "Tile.hpp"

#include <raylib.h>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include <mutex>
#include <vector>


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
    // ryalib related methods
    void ProcessInput();
    void GenerateOutput();
    void ClearGrid();


    // Search related methods and attributes
    bool inBounds(Coordinates& id) const;
    bool passable(Coordinates id) const;
    std::vector<Coordinates> neighbors(Coordinates& id) const;
    void collectObstacles();
    void clearContainers();
    void printPath();

    void Bfs();
    void Dijkstra();
    void AStar();


    std::array<Coordinates, 4> Delta {
        Coordinates{1, 0},  // East
        Coordinates{-1, 0}, // West
        Coordinates{0, -1}, // North
        Coordinates{0, 1}   // South
    };
    std::unordered_map<Coordinates, Coordinates> cameFrom;
    std::unordered_set<Coordinates> obstacles;

    Algorithm algorithm;

    // raylib related attributes
    Vector2 mousePosition;
    TileState originState;
    Tile* startPtr;
    Tile* goalPtr;

    bool startButtonDrag;
    bool goalButtonDrag;
    bool searchExecuted;
    bool isGUIBusy;

    Tile clearButton;
    Tile searchButton;

    std::array<std::array<Tile, MAX_TILES_X>, MAX_TILES_Y> grid;
    // std::array<std::array<int, MAX_TILES_X>, MAX_TILES_Y> colorState;

    // std::mutex clear_mutex;
};
