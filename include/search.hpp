#pragma once

#include <array>
#include <functional>
#include <queue>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "tile.hpp"

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
template <>
struct hash<Coordinates> {
    std::size_t operator()(const Coordinates& id) const noexcept {
        return std::hash<int>()(id.x ^ (id.y << 4));
    }
};
}  // namespace std

class Search {
public:
    Search() = default;
    void Bfs(std::vector<std::vector<Tile>>& grid, Tile* start_ptr, Tile* goal_ptr, bool& gui_busy);
    void Dijkstra(std::vector<std::vector<Tile>>& grid, Tile* start_ptr, Tile* goal_ptr, bool& gui_busy);
    void AStar(std::vector<std::vector<Tile>>& grid, Tile* start_ptr, Tile* goal_ptr, bool& gui_busy);

private:
    bool InBounds(Coordinates& id, std::vector<std::vector<Tile>>& grid) const;
    bool Passable(Coordinates& id) const;
    std::vector<Coordinates> Neighbors(Coordinates& id, std::vector<std::vector<Tile>>& grid) const;
    std::string GetVector(Coordinates& current, Coordinates& from);
    void CollectObstacles(const std::vector<std::vector<Tile>>& grid);
    void SetPath(std::vector<std::vector<Tile>>& grid, const Tile* start_ptr, Tile* goal_ptr);
    double Cost(Coordinates& from_node, Coordinates& to_node) const;
    double Heuristic(const Coordinates& a, const Coordinates& b);
    std::array<Coordinates, 4> delta_{
        Coordinates{1, 0},   // East
        Coordinates{-1, 0},  // West
        Coordinates{0, -1},  // North
        Coordinates{0, 1}    // South
    };
    std::unordered_set<Coordinates> obstacles_;
    std::unordered_map<Coordinates, Coordinates> came_from_;
    std::unordered_map<Coordinates, double> cost_so_far_;
};
