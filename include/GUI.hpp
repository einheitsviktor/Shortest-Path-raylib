#pragma once

#include "Tile.hpp"

#include <queue>
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

constexpr int MAX_TILES_Y = 25; // Rows
constexpr int MAX_TILES_X = 50; // Columns


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
    // GUI related methods
    void ProcessInput();
    void GenerateOutput();
    void ClearGrid();
    void PurgeGrid();


    // Search related methods and attributes
    bool inBounds(Coordinates& id) const;
    bool passable(Coordinates id) const;
    std::vector<Coordinates> neighbors(Coordinates& id) const;
    void collectObstacles();
    void clearContainers();
    void printPath();
    double cost(Coordinates fromNode, Coordinates toNode) const;
    double heuristic(const Coordinates& a, const Coordinates& b);

    // Search algorithms
    void Bfs();
    void Dijkstra();
    void AStar();

    std::array<Coordinates, 4> Delta {
        Coordinates{1, 0},  // East
        Coordinates{-1, 0}, // West
        Coordinates{0, -1}, // North
        Coordinates{0, 1}   // South
    };
    std::unordered_set<Coordinates> obstacles;
    std::unordered_map<Coordinates, Coordinates> cameFrom;
    std::unordered_map<Coordinates, double> costSoFar;

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
};

// Wrapper for own priority queue sort
template<typename T, typename priority_t>
struct PrioriyQueue {
    std::priority_queue<std::pair<priority_t, T>, std::vector<std::pair<priority_t, T>>,
        std::greater<std::pair<priority_t, T>>> elements;
    inline bool empty() const { return elements.empty(); }
    inline void put(T item, priority_t priority) { elements.emplace(priority, item); }
    T get() {
        T ret = elements.top().second;
        elements.pop();
        return ret;
    }
};
