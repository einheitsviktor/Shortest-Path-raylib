#include "search.hpp"

bool Search::InBounds(Coordinates& id, std::vector<std::vector<Tile>>& grid) const {
    return 0 <= id.x && id.x < grid[0].size() && 0 <= id.y && id.y < grid.size();
}

bool Search::Passable(Coordinates& id) const {
    return obstacles_.find(id) == obstacles_.end();
}

std::vector<Coordinates> Search::Neighbors(Coordinates& id, std::vector<std::vector<Tile>>& grid) const {
    std::vector<Coordinates> ret;
    for (const auto& dir : delta_) {
        Coordinates next{id.x + dir.x, id.y + dir.y};
        if (InBounds(next, grid) && Passable(next)) {
            ret.push_back(next);
        }
    }
    // Nudge directions for "prettier" paths
    if ((id.x + id.y) % 2 == 0) {
        std::reverse(ret.begin(), ret.end());
    }
    return ret;
}

std::string Search::GetVector(Coordinates& current, Coordinates& from) {
    if (from.x > current.x) {
        return "A";  // Right arrow
    } else if (from.x < current.x) {
        return "B";  // Left arrow
    } else if (from.y > current.y) {
        return "D";  // Up arrow
    } else {
        return "C";  // Down arrow
    }
}

void Search::CollectObstacles(const std::vector<std::vector<Tile>>& grid) {
    for (const auto& row : grid) {
        for (const auto& col : row) {
            if (col.IsTileObstacle()) {
                obstacles_.insert(Coordinates{col.x, col.y});
            }
        }
    }
}

void Search::SetPath(std::vector<std::vector<Tile>>& grid, const Tile* start_ptr, Tile* goal_ptr) {
    std::vector<Coordinates> path;
    Coordinates current{goal_ptr->x, goal_ptr->y};
    Coordinates start{start_ptr->x, start_ptr->y};
    while (current != start) {
        path.push_back(current);
        current = came_from_[current];
    }
    auto rit = path.rbegin();
    for (; rit != path.rend(); ++rit) {
        grid[rit->y][rit->x].SetTilePath();
        goal_ptr->SetTileGoal();
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}

double Search::Cost(Coordinates& from_node, Coordinates& to_node) const {
    bool nudge = false;
    int x1 = from_node.x, y1 = from_node.y;
    int x2 = to_node.x, y2 = to_node.y;
    if ((x1 + y1) % 2 == 0 && x2 != x1) { nudge = true; }
    if ((x1 + y1) % 2 == 1 && y2 != y1) { nudge = true; }
    return nudge ? 1.001 : 1;
}

double Search::Heuristic(const Coordinates& a, const Coordinates& b) {
    return std::abs(b.x - a.x) + std::abs(b.y - a.y);
}

void Search::Bfs(std::vector<std::vector<Tile>>& grid, Tile* start_ptr, Tile* goal_ptr, bool& gui_busy) {
    // Lock GUI inputs
    gui_busy = true;
    CollectObstacles(grid);

    std::queue<Coordinates> frontier;
    frontier.push(Coordinates{start_ptr->x, start_ptr->y});
    came_from_[Coordinates{start_ptr->x, start_ptr->y}] = Coordinates{start_ptr->x, start_ptr->y};

    while (!frontier.empty()) {
        Coordinates current = frontier.front();
        frontier.pop();
        if (current == Coordinates{goal_ptr->x, goal_ptr->y}) {
            SetPath(grid, start_ptr, goal_ptr);
            break;
        }
        for (Coordinates next : Neighbors(current, grid)) {
            if (came_from_.find(next) == came_from_.end()) {
                frontier.push(next);
                came_from_[next] = current;
                if (grid[next.y][next.x].IsTileGoal()) {
                    goal_ptr->SetTileGoal();
                } else {
                    grid[next.y][next.x].SetTileVisited();
                    grid[next.y][next.x].text = GetVector(next, current);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        }
    }
    // Release GUI processing
    gui_busy = false;
}

void Search::Dijkstra(std::vector<std::vector<Tile>>& grid, Tile* start_ptr, Tile* goal_ptr, bool& gui_busy) {
    // Lock GUI inputs
    gui_busy = true;
    CollectObstacles(grid);

    PrioriyQueue<Coordinates, double> frontier;
    frontier.Put(Coordinates{start_ptr->x, start_ptr->y}, 0);

    came_from_[Coordinates{start_ptr->x, start_ptr->y}] = Coordinates{start_ptr->x, start_ptr->y};
    cost_so_far_[Coordinates{start_ptr->x, start_ptr->y}] = 0;

    while (!frontier.Empty()) {
        Coordinates current = frontier.Get();
        if (current == Coordinates{goal_ptr->x, goal_ptr->y}) {
            SetPath(grid, start_ptr, goal_ptr);
            break;
        }
        for (Coordinates next : Neighbors(current, grid)) {
            double new_cost = cost_so_far_[current] + Cost(current, next);
            if (cost_so_far_.find(next) == cost_so_far_.end() || new_cost < cost_so_far_[next]) {
                cost_so_far_[next] = new_cost;
                came_from_[next] = current;
                frontier.Put(next, new_cost);
                if (grid[next.y][next.x].IsTileGoal()) {
                    goal_ptr->SetTileGoal();
                } else {
                    grid[next.y][next.x].SetTileVisited();
                    grid[next.y][next.x].text = GetVector(next, current);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        }
    }
    // Release GUI processing
    gui_busy = false;
}

void Search::AStar(std::vector<std::vector<Tile>>& grid, Tile* start_ptr, Tile* goal_ptr, bool& gui_busy) {
    // Lock GUI inputs
    gui_busy = true;
    CollectObstacles(grid);

    PrioriyQueue<Coordinates, double> frontier;
    frontier.Put(Coordinates{start_ptr->x, start_ptr->y}, 0);

    came_from_[Coordinates{start_ptr->x, start_ptr->y}] = Coordinates{start_ptr->x, start_ptr->y};
    cost_so_far_[Coordinates{start_ptr->x, start_ptr->y}] = 0;

    while (!frontier.Empty()) {
        Coordinates current = frontier.Get();
        if (current == Coordinates{goal_ptr->x, goal_ptr->y}) {
            SetPath(grid, start_ptr, goal_ptr);
            break;
        }
        for (Coordinates next : Neighbors(current, grid)) {
            double new_cost = cost_so_far_[current] + Cost(current, next);
            if (cost_so_far_.find(next) == cost_so_far_.end() || new_cost < cost_so_far_[next]) {
                cost_so_far_[next] = new_cost;
                double priority = new_cost + Heuristic(next, Coordinates{goal_ptr->x, goal_ptr->y});
                frontier.Put(next, priority);
                came_from_[next] = current;
                if (grid[next.y][next.x].IsTileGoal()) {
                    goal_ptr->SetTileGoal();
                } else {
                    grid[next.y][next.x].SetTileVisited();
                    grid[next.y][next.x].text = GetVector(next, current);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(6));
            }
        }
    }
    // Release GUI processing
    gui_busy = false;
}
