#include "GUI.hpp"

#include <cmath>
#include <raylib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

// Constructor
GUI::GUI()
    : algorithm(Algorithm::Bfs)
    , mousePosition(Vector2{ 0.0f, 0.0f })
    , originState(TileState::empty)
    , startPtr(nullptr)
    , goalPtr(nullptr)
    , startButtonDrag(false)
    , goalButtonDrag(false)
    , searchExecuted(false)
    , isGUIBusy(false)
{
    // Set GUI fps
    SetTargetFPS(60);
    // Set GUI width and height
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shortest Path raylib");

    // Initialize tile positions in grid
    for (int i = 0, max_tiles = MAX_TILES_X * MAX_TILES_Y; i < max_tiles; ++i) {
        int y = i / (max_tiles/MAX_TILES_Y);
        int x = i % MAX_TILES_X;
        int recX = 30.0f  + TILE_WIDTH * (i % MAX_TILES_X) + 1.0f * (i % MAX_TILES_X); 
        int recY = 150.0f + TILE_WIDTH * (i / MAX_TILES_X) + 1.0f * (i / MAX_TILES_X);
        this->grid[y][x] = Tile(y, x, recY, recX, TILE_WIDTH, TILE_WIDTH);
    }
    //         y  x
    this->grid[3][3].setTileStart();
    this->grid[21][46].setTileGoal();
    this->startPtr = &this->grid[3][3];
    this->goalPtr = &this->grid[21][46];

    // Initialize upper buttons
    int bfs = 650;
    int djk = bfs + 120;
    int ast = djk + 120;
    this->BfsButton =       Tile{0, 0, 50, bfs, 110, 40};
    this->DijkstraButton =  Tile{0, 0, 50, djk, 110, 40};
    this->AStarButton =     Tile{0, 0, 50, ast, 110, 40};

    this->clearButton =     Tile(0, 0, 50, 1090, 100, 40);
    this->searchButton =    Tile(0, 0, 50, 1200, 120, 40);
}

// Destructor
GUI::~GUI() { CloseWindow(); }

void GUI::RunLoop() {
    while (!WindowShouldClose()) {
        ProcessInput();
        GenerateOutput();
    }
}

// Inside while loop
void GUI::ProcessInput() {
    // Do not accecpt input if a search is currently executing
    if (this->isGUIBusy) return;
    this->mousePosition = GetMousePosition();

    // Process algorithm buttons
    if (CheckCollisionPointRec(this->mousePosition, this->BfsButton.rec)) {
        this->BfsButton.setButtonHover();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            this->BfsButton.setButtonPressed();
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            this->algorithm = Algorithm::Bfs;
        else
            this->BfsButton.setButtonHover();
    }
    else this->BfsButton.setButtonNormal();
    
    // Process Dijkstra button
    if (CheckCollisionPointRec(this->mousePosition, this->DijkstraButton.rec)) {
        this->DijkstraButton.setButtonHover();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            this->DijkstraButton.setButtonPressed();
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            this->algorithm = Algorithm::Dijkstra;
        else
            this->DijkstraButton.setButtonHover();
    }
    else this->DijkstraButton.setButtonNormal();

    // Process AStar button
    if (CheckCollisionPointRec(this->mousePosition, this->AStarButton.rec)) {
        this->AStarButton.setButtonHover();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            this->AStarButton.setButtonPressed();
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            this->algorithm = Algorithm::AStar;
        else
            this->AStarButton.setButtonHover();
    }
    else this->AStarButton.setButtonNormal();

    // Process Clear button
    if (CheckCollisionPointRec(this->mousePosition, this->clearButton.rec)) {
        this->clearButton.setButtonHover();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            this->clearButton.setButtonPressed();
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            this->ClearGrid();
        else
            this->clearButton.setButtonHover();
    }
    else this->clearButton.setButtonNormal();

    // Process Sesarch button
    if (CheckCollisionPointRec(this->mousePosition, this->searchButton.rec)) {
        this->searchButton.setButtonHover();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            this->searchButton.setButtonPressed();
        }
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (this->searchExecuted)
                this->PurgeGrid();
            // Detach search algorithm as an own thread
            if (this->algorithm == Algorithm::Bfs) {
                std::thread search(&GUI::Bfs, this);
                search.detach();
            }
            else if (this->algorithm == Algorithm::Dijkstra) {
                std::thread search(&GUI::Dijkstra, this);
                search.detach();
            }
            else {
                std::thread search(&GUI::AStar, this);
                search.detach();
            }
        }
        else {
            this->searchButton.setButtonHover();
        }
    }
    else this->searchButton.setButtonNormal();
    
    // Process grid
    for (auto& row : this->grid) {
        for (auto& col : row) {
            if (CheckCollisionPointRec(this->mousePosition, col.rec)) {
                // Use left mouse button to place obstacles or drag and drop start and goal
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    if (this->searchExecuted) {
                        this->PurgeGrid();
                        this->searchExecuted = false;
                    }
                    if (this->startButtonDrag) {
                        if (col.isEmpty()) {
                            col.setTileStart();
                            this->startPtr->setTileEmpty();
                            this->startPtr = &col;
                        }
                    } else if (this->goalButtonDrag) {
                        if (col.isEmpty()) {
                            col.setTileGoal();
                            this->goalPtr->setTileEmpty();
                            this->goalPtr =  &col;
                        }
                    } else if (col.isEmpty()) {
                        col.setTileObstacle();
                    } else if (col.isStart()) {
                        this->startButtonDrag = true;
                    } else if (col.isGoal()) {
                        this->goalButtonDrag = true;
                    }

                }
                // Use right mouse button to erase obstacles
                else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    if (col.isObstacle()) {
                        if (this->searchExecuted) {
                            this->PurgeGrid();
                            this->searchExecuted = false;
                        }
                        col.setTileEmpty();
                    }
                }
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                this->startButtonDrag = false;
                this->goalButtonDrag = false;
            }
        }
    }
}


void GUI::GenerateOutput() {
    // Offset for text in buttons
    static int bfsX = this->BfsButton.rec.x + 40,     bfsY = this->BfsButton.rec.y + 10;
    static int djkX = this->DijkstraButton.rec.x + 8, djkY = this->DijkstraButton.rec.y + 10;
    static int astX = this->AStarButton.rec.x + 23,   astY = this->AStarButton.rec.y + 10;
    static int fontSize = 24;

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        // Draw outline box around algorithm buttons
        DrawRectangleLinesEx(this->getTileToOutline(), 2.0f, BLACK);

        // Draw Bfs button
        DrawRectangleRec(this->BfsButton.rec, Fade(VIOLET, 0.2f));
        DrawText("Bfs", bfsX, bfsY, fontSize, BLACK);
        if (this->BfsButton.buttonPressed()) {
            DrawRectangleRec(this->BfsButton.rec, Fade(VIOLET, 0.4f));
            DrawText("Bfs", bfsX, bfsY, fontSize, BLACK);
        }
        else if (this->BfsButton.buttonHover()) {
            DrawRectangleRec(this->BfsButton.rec, Fade(VIOLET, 0.4f));
            DrawText("Bfs", bfsX, bfsY, fontSize, BLACK);
        }

        // Draw Dijkstra button
        DrawRectangleRec(this->DijkstraButton.rec, Fade(VIOLET, 0.2f));
        DrawText("Dijkstra", djkX, djkY, fontSize, BLACK);
        if (this->DijkstraButton.buttonPressed()) {
            DrawRectangleRec(this->DijkstraButton.rec, Fade(VIOLET, 0.4f));
            DrawText("Dijkstra", djkX, djkY, fontSize, BLACK);
        } else if (this->DijkstraButton.buttonHover()) {
            DrawRectangleRec(this->DijkstraButton.rec, Fade(VIOLET, 0.4f));
            DrawText("Dijkstra", djkX, djkY, fontSize, BLACK);
        }

        // Draw AStar button
        DrawRectangleRec(this->AStarButton.rec, Fade(VIOLET, 0.2f));
        DrawText("AStar", astX, astY, fontSize, BLACK);
        if (this->AStarButton.buttonPressed()) {
            DrawRectangleRec(this->AStarButton.rec, Fade(VIOLET, 0.4f));
            DrawText("AStar", astX, astY, fontSize, BLACK);
        } else if (this->AStarButton.buttonHover()) {
            DrawRectangleRec(this->AStarButton.rec, Fade(VIOLET, 0.4f));
            DrawText("AStar", astX, astY, fontSize, BLACK);
        }

        // Draw clearButton
        DrawRectangleRec(this->clearButton.rec, Fade(SKYBLUE, 0.5f));
        DrawText("Clear", this->clearButton.rec.x+20, this->clearButton.rec.y+10, fontSize, BLACK);
        if (this->clearButton.buttonPressed()) {
            DrawRectangleRec(this->clearButton.rec, Fade(SKYBLUE, 0.4f));
            DrawText("Clear", this->clearButton.rec.x+20, this->clearButton.rec.y+10, fontSize, BLACK);
        } else if (this->clearButton.buttonHover()) {
            DrawRectangleRec(this->clearButton.rec, Fade(SKYBLUE, 0.41f));
            DrawText("Clear", this->clearButton.rec.x+20, this->clearButton.rec.y+10, fontSize, BLACK);
        }

        // Draw searchButton
        DrawRectangleRec(this->searchButton.rec, Fade(DARKGREEN, 0.5f));
        DrawText("Search", this->searchButton.rec.x+20, this->searchButton.rec.y+10, fontSize, BLACK);
        if (this->searchButton.buttonPressed()) {
            DrawRectangleRec(this->searchButton.rec, Fade(DARKGREEN, 1.0f));
            DrawText("Search", this->searchButton.rec.x+20, this->searchButton.rec.y+10, fontSize, BLACK);
        } else if (this->searchButton.buttonHover()) {
            DrawRectangleRec(this->searchButton.rec, Fade(DARKGREEN, 0.5f));
            DrawText("Search", this->searchButton.rec.x+20, this->searchButton.rec.y+10, fontSize, BLACK);
        }


        // Draw grid
        for (const auto& row : this->grid) {
            for (const auto& col : row) {
                if (col.isObstacle())
                    DrawRectangleRec(col.rec, Fade(BLACK, 0.7f));
                else if (col.isStart()) {
                    DrawRectangleRec(col.rec, GREEN);
                    DrawText("S", col.rec.x+5, col.rec.y+2, fontSize, DARKBROWN);
                }
                else if (col.isGoal()) {
                    DrawRectangleRec(col.rec, RED);
                    DrawText("G", col.rec.x+5, col.rec.y+2, fontSize, BLACK);
                } 
                else if (col.isVisited())
                    DrawRectangleRec(col.rec, Fade(DARKBLUE, 0.3f));
                else if (col.isPath())
                    DrawRectangleRec(col.rec, Fade(GOLD, 0.5f));
                else
                    DrawRectangleRec(col.rec, Fade(LIGHTGRAY, 0.5f));
            }
        }
    }
    EndDrawing();
}

void GUI::ClearGrid() {
    for (auto& row : this->grid) {
        for (auto& col : row) {
            if (!col.isStart() && !col.isGoal()) {
                col.setTileEmpty();
            }
        }
    }
    this->searchExecuted = false;
}

void GUI::PurgeGrid() {
    for (auto& row : this->grid) {
        for (auto& col : row) {
            if (col.isPath() || col.isVisited()) {
                col.setTileEmpty();
            }
        }
    }
    this->searchExecuted = false;
}

Rectangle GUI::getTileToOutline() {
    if (this->algorithm == Algorithm::Bfs) return this->BfsButton.rec;
    else if (this->algorithm == Algorithm::Dijkstra) return this->DijkstraButton.rec;
    else return this->AStarButton.rec;
}

// Search algorithm related code
bool GUI::inBounds(Coordinates& id) const {
    return 0 <= id.x && id.x < this->grid[0].size() && 0 <= id.y && id.y < this->grid.size();
}

bool GUI::passable(Coordinates id) const {
    return this->obstacles.find(id) == this->obstacles.end();
}

std::vector<Coordinates> GUI::neighbors(Coordinates& id) const {
    std::vector<Coordinates> ret;
    for (const auto& dir : this->Delta) {
        Coordinates next{id.x + dir.x, id.y + dir.y};
        if (inBounds(next) && passable(next)) {
            ret.push_back(next);
        }
    }
    // Nudge directions for "prettier" paths
    if ((id.x + id.y) % 2 == 0)
        std::reverse(ret.begin(), ret.end());
    return ret;
}

void GUI::collectObstacles() {
    for (const auto& row : this->grid) {
        for (const auto& col : row) {
            if (col.isObstacle())
                this->obstacles.insert(Coordinates{col.x, col.y});
        }
    }
}

void GUI::clearContainers() {
    this->obstacles.clear();
    this->cameFrom.clear();
    this->costSoFar.clear();
}

void GUI::printPath() {
    std::vector<Coordinates> path;
    Coordinates current{this->goalPtr->x, this->goalPtr->y};
    Coordinates start{this->startPtr->x, this->startPtr->y};
    while (current != start) {
        path.push_back(current);
        current = this->cameFrom[current];
    }
    auto rit = path.rbegin();
    for (; rit != path.rend(); ++rit) {
        this->grid[rit->y][rit->x].setTilePath();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

double GUI::cost(Coordinates fromNode, Coordinates toNode) const {
    bool nudge = false;
    int x1 = fromNode.x, y1 = fromNode.y;
    int x2 = toNode.x, y2 = toNode.y;
    if ((x1 + y1) % 2 == 0 && x2 != x1) nudge = true;
    if ((x1 + y1) % 2 == 1 && y2 != y1) nudge = true;
    return  nudge ? 1.001 : 1;
}

double GUI::heuristic(const Coordinates& a, const Coordinates& b) {
    return std::abs(b.x - a.x) + std::abs(b.y - a.y);
}

void GUI::Bfs() {
    // Lock GUI inputs
    this->isGUIBusy = true;
    std::mutex mut;

    this->clearContainers();
    this->collectObstacles();
    
    std::queue<Coordinates> frontier;
    frontier.push(Coordinates{this->startPtr->x, this->startPtr->y});

    this->cameFrom[Coordinates{this->startPtr->x, this->startPtr->y}] = Coordinates{this->startPtr->x, this->startPtr->y};

    while (!frontier.empty()) {
        Coordinates current = frontier.front();
        frontier.pop();
        if (current == Coordinates{this->goalPtr->x, this->goalPtr->y}) {
            this->printPath();
            break;
        }
        for (Coordinates next : this->neighbors(current)) {
            if (this->cameFrom.find(next) == this->cameFrom.end()) {
                frontier.push(next);
                this->cameFrom[next] = current;
                mut.lock();
                    if (this->grid[next.y][next.x].isGoal())
                        this->goalPtr->setTileGoal();
                    else
                        this->grid[next.y][next.x].setTileVisited();
                mut.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        }
    }
    this->goalPtr->setTileGoal();
    // Release GUI processing
    this->clearContainers();
    this->isGUIBusy = false;
    this->searchExecuted = true;
}

void GUI::Dijkstra() {
    // Lock GUI inputs
    this->isGUIBusy = true;
    std::mutex mut;

    this->clearContainers();
    this->collectObstacles();

    PrioriyQueue<Coordinates, double> frontier;
    frontier.put(Coordinates{this->startPtr->x, this->startPtr->y}, 0);

    this->cameFrom[Coordinates{this->startPtr->x, this->startPtr->y}] = Coordinates{this->startPtr->x, this->startPtr->y};
    this->costSoFar[Coordinates{this->startPtr->x, this->startPtr->y}] = 0;

    while (!frontier.empty()) {
        Coordinates current = frontier.get();
        if (current == Coordinates{this->goalPtr->x, this->goalPtr->y}) {
            this->printPath();
            break;
        }
        for (Coordinates next : this->neighbors(current)) {
            double newCost = this->costSoFar[current] + this->cost(current, next);
            if (this->costSoFar.find(next) == this->costSoFar.end() || newCost < this->costSoFar[next]) {
                this->costSoFar[next] = newCost;
                this->cameFrom[next] = current;
                frontier.put(next, newCost);
                mut.lock();
                    if (this->grid[next.y][next.x].isGoal())
                        this->goalPtr->setTileGoal();
                    else
                        this->grid[next.y][next.x].setTileVisited();
                mut.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        }
    }
    this->goalPtr->setTileGoal();
    // Release GUI processing
    this->clearContainers();
    this->isGUIBusy = false;
    this->searchExecuted = true;
}

void GUI::AStar() {
    // Lock GUI inputs
    this->isGUIBusy = true;
    std::mutex mut;

    this->clearContainers();
    this->collectObstacles();

    PrioriyQueue<Coordinates, double> frontier;
    frontier.put(Coordinates{this->startPtr->x, this->startPtr->y}, 0);

    this->cameFrom[Coordinates{this->startPtr->x, this->startPtr->y}] = Coordinates{this->startPtr->x, this->startPtr->y};
    this->costSoFar[Coordinates{this->startPtr->x, this->startPtr->y}] = 0;

    while (!frontier.empty()) {
        Coordinates current = frontier.get();
        if (current == Coordinates{this->goalPtr->x, this->goalPtr->y}) {
            this->printPath();
            break;
        }
        for (Coordinates next : this->neighbors(current)) {
            double newCost = this->costSoFar[current] + this->cost(current, next);
            if (this->costSoFar.find(next) == this->costSoFar.end() || newCost < this->costSoFar[next]) {
                this->costSoFar[next] = newCost;
                double priority = newCost + this->heuristic(next, Coordinates{this->goalPtr->x, this->goalPtr->y});
                frontier.put(next, priority);
                this->cameFrom[next] = current;
                mut.lock();
                    if (this->grid[next.y][next.x].isGoal())
                        this->goalPtr->setTileGoal();
                    else
                        this->grid[next.y][next.x].setTileVisited();
                mut.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
    }
    this->goalPtr->setTileGoal();
    // Release GUI processing
    this->clearContainers();
    this->isGUIBusy = false;
    this->searchExecuted = true;
}
