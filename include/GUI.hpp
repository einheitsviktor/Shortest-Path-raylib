#pragma once

#include "Tile.hpp"

#include <iostream>
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
    Rectangle getTileToOutline();
    void printPreset(const std::vector<std::vector<int>>& vec);


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

    // GUI related attributes
    Vector2 mousePosition;
    TileState originState;
    Tile* startPtr;
    Tile* goalPtr;

    Tile presetButton1;
    Tile presetButton2;
    Tile presetButton3;

    Tile BfsButton;
    Tile DijkstraButton;
    Tile AStarButton;

    Tile clearButton;
    Tile searchButton;

    bool startButtonDrag;
    bool goalButtonDrag;
    bool searchExecuted;
    bool isGUIBusy;

    std::array<std::array<Tile, MAX_TILES_X>, MAX_TILES_Y> grid;

    // TODO: Delete helper
    void printObstacles() {
        for (int i = 0; i < this->grid.size(); ++i) {
            for (int j = 0; j < this->grid[0].size(); ++j) {
                if (this->grid[i][j].isObstacle()) {
                    std::cout << "{" << i << ", " << j << "},\n";
                }
            }
        }
    }
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

// Boilerplate coordinates for presets
const std::vector<std::vector<int>> preset1 {
    {0, 38}, {0, 39}, {0, 40}, {0, 41}, {1, 38}, {1, 39}, {1, 40}, {1, 41}, {2, 38}, {2, 39},
    {2, 40}, {2, 41}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8}, {3, 9}, {3, 38}, {3, 39},
    {3, 40}, {3, 41}, {4, 3}, {4, 4}, {4, 5}, {4, 6}, {4, 7}, {4, 8}, {4, 9}, {4, 38}, {4, 39},
    {4, 40}, {4, 41}, {5, 3}, {5, 4}, {5, 5}, {5, 6}, {5, 7}, {5, 8}, {5, 9}, {5, 38}, {5, 39},
    {5, 40}, {5, 41}, {6, 3}, {6, 4}, {6, 5}, {6, 6}, {6, 7}, {6, 8}, {6, 9}, {6, 18}, {6, 19},
    {6, 20}, {6, 21}, {6, 22}, {6, 23}, {6, 24}, {6, 25}, {6, 26}, {6, 38}, {6, 39}, {6, 40},
    {6, 41}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {7, 7}, {7, 8}, {7, 9}, {7, 18}, {7, 19}, {7, 20},
    {7, 21}, {7, 22}, {7, 23}, {7, 24}, {7, 25}, {7, 26}, {7, 38}, {7, 39}, {7, 40}, {7, 41},
    {8, 3}, {8, 4}, {8, 5}, {8, 6}, {8, 7}, {8, 8}, {8, 9}, {8, 18}, {8, 19}, {8, 20}, {8, 21},
    {8, 22}, {8, 23}, {8, 24}, {8, 25}, {8, 26}, {8, 38}, {8, 39}, {8, 40}, {8, 41}, {8, 42},
    {8, 43}, {8, 44}, {8, 45}, {9, 3}, {9, 4}, {9, 5}, {9, 6}, {9, 7}, {9, 8}, {9, 9}, {9, 18},
    {9, 19}, {9, 20}, {9, 21}, {9, 22}, {9, 23}, {9, 24}, {9, 25}, {9, 26}, {9, 38}, {9, 39},
    {9, 40}, {9, 41}, {9, 42}, {9, 43}, {9, 44}, {9, 45}, {10, 3}, {10, 4}, {10, 5}, {10, 6},
    {10, 7}, {10, 8}, {10, 9}, {10, 18}, {10, 19}, {10, 20}, {10, 21}, {10, 22}, {10, 23}, {10, 24},
    {10, 25}, {10, 26}, {10, 38}, {10, 39}, {10, 40}, {10, 41}, {10, 42}, {10, 43}, {10, 44},
    {10, 45}, {11, 3}, {11, 4}, {11, 5}, {11, 6}, {11, 7}, {11, 8}, {11, 9}, {11, 18}, {11, 19},
    {11, 20}, {11, 21}, {11, 22}, {11, 23}, {11, 24}, {11, 25}, {11, 26}, {11, 38}, {11, 39},
    {11, 40}, {11, 41}, {11, 42}, {11, 43}, {11, 44}, {11, 45}, {12, 3}, {12, 4}, {12, 5}, {12, 6},
    {12, 7}, {12, 8}, {12, 9}, {12, 18}, {12, 19}, {12, 20}, {12, 21}, {12, 22}, {12, 23}, {12, 24},
    {12, 25}, {12, 26}, {13, 3}, {13, 4}, {13, 5}, {13, 6}, {13, 7}, {13, 8}, {13, 9}, {13, 18},
    {13, 19}, {13, 20}, {13, 21}, {13, 22}, {13, 23}, {13, 24}, {13, 25}, {13, 26}, {14, 3}, {14, 4},
    {14, 5}, {14, 6}, {14, 7}, {14, 8}, {14, 9}, {14, 18}, {14, 19}, {14, 20}, {14, 21}, {14, 22},
    {14, 23}, {14, 24}, {14, 25}, {14, 26}, {15, 3}, {15, 4}, {15, 5}, {15, 6}, {15, 7}, {15, 8},
    {15, 9}, {15, 18}, {15, 19}, {15, 20}, {15, 21}, {15, 22}, {15, 23}, {15, 24}, {15, 25}, {15, 26},
    {16, 3}, {16, 4}, {16, 5}, {16, 6}, {16, 7}, {16, 8}, {16, 9}, {16, 18}, {16, 19}, {16, 20},
    {16, 21}, {16, 22}, {16, 23}, {16, 24}, {16, 25}, {16, 26}, {17, 3}, {17, 4}, {17, 5}, {17, 6},
    {17, 7}, {17, 8}, {17, 9}, {17, 18}, {17, 19}, {17, 20}, {17, 21}, {17, 22}, {17, 23}, {17, 24},
    {17, 25}, {17, 26}, {18, 3}, {18, 4}, {18, 5}, {18, 6}, {18, 7}, {18, 8}, {18, 9}, {18, 18},
    {18, 19}, {18, 20}, {18, 21}, {18, 22}, {18, 23}, {18, 24}, {18, 25}, {18, 26}, {19, 3}, {19, 4},
    {19, 5}, {19, 6}, {19, 7}, {19, 8}, {19, 9}, {19, 18}, {19, 19}, {19, 20}, {19, 21}, {19, 22},
    {19, 23}, {19, 24}, {19, 25}, {19, 26}, {20, 3}, {20, 4}, {20, 5}, {20, 6}, {20, 7}, {20, 8},
    {20, 9}, {20, 18}, {20, 19}, {20, 20}, {20, 21}, {20, 22}, {20, 23}, {20, 24}, {20, 25}, {20, 26},
    {21, 18}, {21, 19}, {21, 20}, {21, 21}, {21, 22}, {21, 23}, {21, 24}, {21, 25}, {21, 26}, {22, 18},
    {22, 19}, {22, 20}, {22, 21}, {22, 22}, {22, 23}, {22, 24}, {22, 25}, {22, 26}, {23, 18}, {23, 19},
    {23, 20}, {23, 21}, {23, 22}, {23, 23}, {23, 24}, {23, 25}, {23, 26}, {24, 18}, {24, 19}, {24, 20},
    {24, 21}, {24, 22}, {24, 23}, {24, 24}, {24, 25}, {24, 26}
};

const std::vector<std::vector<int>> preset2 {
    {6, 19}, {6, 20}, {6, 21}, {6, 22}, {6, 23}, {6, 24}, {6, 25}, {6, 26}, {6, 27}, {6, 28}, {6, 29},
    {6, 30}, {7, 30}, {8, 30}, {9, 30}, {10, 30}, {11, 30}, {12, 30}, {13, 30}, {14, 30}, {15, 30},
    {16, 30}, {17, 30}, {18, 14}, {18, 15}, {18, 16}, {18, 17}, {18, 18}, {18, 19}, {18, 20}, {18, 21},
    {18, 22}, {18, 23}, {18, 24}, {18, 25}, {18, 26}, {18, 27}, {18, 28}, {18, 29}, {18, 30}
};

const std::vector<std::vector<int>> preset3 {
    {6, 19}, {6, 29}, {7, 20}, {7, 28}, {8, 21}, {8, 27}, {9, 22}, {9, 26}, {10, 23}, {10, 25}, {11, 24},
    {12, 23}, {12, 25}, {13, 22}, {13, 26}, {14, 21}, {14, 27}, {15, 20}, {15, 28}, {16, 19}, {16, 29}
};
