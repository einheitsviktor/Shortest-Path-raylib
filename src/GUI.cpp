#include "GUI.hpp"

#include <raylib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore>
#include <vector>
#include <queue>

// Constructor
GUI::GUI()
    : algorithm(Algorithm::Bfs)
    , mousePosition(Vector2{ 0.0f, 0.0f })
    , originState(TileState::none)
    , startPtr(nullptr)
    , goalPtr(nullptr)
    , startButtonDrag(false)
    , goalButtonDrag(false)
    , searchExecuted(false)
{
    // Set GUI fps
    SetTargetFPS(60);
    // Set GUI width and height
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shortest Path raylib");

    std::array<Coordinates, 4> Delta {
        Coordinates{1, 0},  // East
        Coordinates{-1, 0}, // West
        Coordinates{0, -1}, // North
        Coordinates{0, 1}   // South
    };


    // Initialize tile positions in grid
    for (int i = 0, max_tiles = MAX_TILES_X * MAX_TILES_Y; i < max_tiles; ++i) {
        // this->grid[i/(max_tiles/MAX_TILES_Y)][i%MAX_TILES_X].rec.x      = 30.0f  + TILE_WIDTH * (i % MAX_TILES_X) + 1.0f * (i % MAX_TILES_X);
        // this->grid[i/(max_tiles/MAX_TILES_Y)][i%MAX_TILES_X].rec.y      = 150.0f + TILE_WIDTH * (i / MAX_TILES_X) + 1.0f * (i / MAX_TILES_X);
        // this->grid[i/(max_tiles/MAX_TILES_Y)][i%MAX_TILES_X].rec.width  = TILE_WIDTH;
        // this->grid[i/(max_tiles/MAX_TILES_Y)][i%MAX_TILES_X].rec.height = TILE_HEIGHT;

        int y = i / (max_tiles/MAX_TILES_Y);
        int x = i % MAX_TILES_X;
        int recX = 30.0f  + TILE_WIDTH * (i % MAX_TILES_X) + 1.0f * (i % MAX_TILES_X); 
        int recY = 150.0f + TILE_WIDTH * (i / MAX_TILES_X) + 1.0f * (i / MAX_TILES_X);
        this->grid[y][x] = Tile(y, x, recY, recX, TILE_WIDTH, TILE_WIDTH);
    }

    //         y  x
    this->grid[3][3].tileState   = TileState::start;
    this->grid[21][46].tileState = TileState::goal;
    this->startPtr = &this->grid[3][3];
    this->goalPtr = &this->grid[21][46];

    // Initialize the reset button
    this->clearButton = Tile(0, 0, 50, 1040, 100, 40);

    // this->clearButton.rec.x = 1040;
    // this->clearButton.rec.y = 50;
    // this->clearButton.rec.width = 100;
    // this->clearButton.rec.height = 40;

    this->searchButton = Tile(0, 0, 50, 1150, 120, 40);

    // this->searchButton.rec.x = 1150;
    // this->searchButton.rec.y = 50;
    // this->searchButton.rec.width = 120;
    // this->searchButton.rec.height = 40;
}

// Destructor
GUI::~GUI() { CloseWindow(); }

void GUI::RunLoop() {
    while (!WindowShouldClose()) {
        ProcessInput();
        // GenerateOutput();
    }
}

// Inside while loop
void GUI::ProcessInput() {
    this->mousePosition = GetMousePosition();
    // this->btnAction = false;

    // Process Clear and Search buttons
    if (CheckCollisionPointRec(this->mousePosition, this->clearButton.rec)) {
        this->clearButton.buttonState = ButtonState::mouse_hover;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            this->clearButton.buttonState = ButtonState::pressed;
        }
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            // std::thread clear(&GUI::Clear, this);
            // clear.detach();
            this->ClearGrid();
        } else {
            this->clearButton.buttonState = ButtonState::mouse_hover;
        }
    }
    else this->clearButton.buttonState = ButtonState::normal;

    if (CheckCollisionPointRec(this->mousePosition, this->searchButton.rec)) {
        this->searchButton.buttonState = ButtonState::mouse_hover;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            this->searchButton.buttonState = ButtonState::pressed;
        } else {
            this->searchButton.buttonState = ButtonState::mouse_hover;
        }
    }
    else this->searchButton.buttonState = ButtonState::normal;
    
    // Process grid
    for (auto& row : this->grid) {
        for (auto& col : row) {
            if (CheckCollisionPointRec(this->mousePosition, col.rec)) {
                // Use left mouse button to place obstacles or drag and drop start and goal
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    if (this->startButtonDrag) {
                        if (col.isEmpty()) {
                            col.tileState = TileState::start;
                            this->startPtr->tileState = TileState::empty;
                            this->startPtr = &col;
                        }
                    } else if (this->goalButtonDrag) {
                        if (col.isEmpty()) {
                            col.tileState = TileState::goal;
                            this->goalPtr->tileState = TileState::empty;
                            this->goalPtr =  &col;
                        }
                    } else if (col.isEmpty()) {
                        col.tileState = TileState::obstacle;
                    } else if (col.isStart()) {
                        this->startButtonDrag = true;
                    } else if (col.isGoal()) {
                        this->goalButtonDrag = true;
                    }

                }
                // Use right mouse button to erase obstacles
                else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    if (col.isObstacle()) {
                        col.tileState = TileState::empty;
                    }
                }
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                this->startButtonDrag = false;
                this->goalButtonDrag = false;
            }
        }
    }
    // TODO: Call search algorithms from here
    this->GenerateOutput();
}


void GUI::GenerateOutput() {
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        // Draw clearButton
        DrawRectangleRec(this->clearButton.rec, Fade(SKYBLUE, 0.5f));
        DrawText("Clear", this->clearButton.rec.x+20, this->clearButton.rec.y+10, 24, BLACK);
        if (this->clearButton.buttonState == ButtonState::pressed) {
            DrawRectangleRec(this->clearButton.rec, Fade(SKYBLUE, 1.0f));
            DrawText("Clear", this->clearButton.rec.x+20, this->clearButton.rec.y+10, 24, BLACK);
        } else if (this->clearButton.buttonState == ButtonState::mouse_hover) {
            DrawRectangleRec(this->clearButton.rec, Fade(SKYBLUE, 0.51f));
            DrawText("Clear", this->clearButton.rec.x+20, this->clearButton.rec.y+10, 24, BLACK);
        }

        // Draw searchButton
        DrawRectangleRec(this->searchButton.rec, Fade(DARKGREEN, 0.5f));
        DrawText("Search", this->searchButton.rec.x+20, this->searchButton.rec.y+10, 24, BLACK);
        if (this->searchButton.buttonState == ButtonState::pressed) {
            DrawRectangleRec(this->searchButton.rec, Fade(DARKGREEN, 1.0f));
            DrawText("Search", this->searchButton.rec.x+20, this->searchButton.rec.y+10, 24, BLACK);
        } else if (this->searchButton.buttonState == ButtonState::mouse_hover) {
            DrawRectangleRec(this->searchButton.rec, Fade(DARKGREEN, 0.5f));
            DrawText("Search", this->searchButton.rec.x+20, this->searchButton.rec.y+10, 24, BLACK);
        }

        for (const auto& row : this->grid) {
            for (const auto& col : row) {
                if (col.isObstacle()) {
                    DrawRectangleRec(col.rec, Fade(BLACK, 0.7f));
                } else if (col.isStart()) {
                    DrawRectangleRec(col.rec, GREEN);
                    DrawText("S", col.rec.x+5, col.rec.y+2, 24, DARKBROWN);
                } else if (col.isGoal()) {
                    DrawRectangleRec(col.rec, RED);
                    DrawText("G", col.rec.x+5, col.rec.y+2, 24, BLACK);
                } else {
                    DrawRectangleRec(col.rec, Fade(LIGHTGRAY, 0.5f));
                }
            }
        }
    }
    EndDrawing();
}

void GUI::ClearGrid() {
    // static std::mutex mut;
    for (auto& row : this->grid) {
        for (auto& col : row) {
            if (!col.isStart() && !col.isGoal()) {
                // mut.lock();
                col.tileState = TileState::empty;
                // mut.unlock();
                // std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
            }
        }
    }
    this->searchExecuted = false;
}

bool GUI::inBounds(Coordinates& id) const {
    return 0 <= id.x && id.x < this->grid[0].size() && 0 <= id.y && id.y < this->grid.size();
}

bool GUI::passable(Coordinates& id) const {
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
    if ((id.x + id.y) % 2 == 0) {
        std::reverse(ret.begin(), ret.end());
    }
    return ret;
}

void GUI::collectObstacles() {
    for (const auto& row : this->grid) {
        for (const auto& col : row) {
            if (col.isObstacle()) {
                this->obstacles.insert(Coordinates{col.y, col.x});
            }
        }
    }
}

void GUI::clearContainers() {
    this->obstacles.clear();
    this->cameFrom.clear();
    // this->costSoFar.clear();
}

void GUI::Bfs() {
    this->clearContainers();
    this->collectObstacles();
    
    std::queue<Coordinates> frontier;
    frontier.push(Coordinates{this->startPtr->y, this->startPtr->x});

    this->cameFrom[Coordinates{this->startPtr->y, this->startPtr->x}] = Coordinates{this->startPtr->y, this->startPtr->x};

    while (!frontier.empty()) {
        Coordinates current = frontier.front();
        frontier.pop();

        if (current == Coordinates{this->goalPtr->y, this->goalPtr->x}) {
            // TODO: implement printPath/setPath
            std::cout << "Path found!\n";
            break;
        }

        for (Coordinates next : this->neighbors(current)) {
            if (this->cameFrom.find(next) == this->cameFrom.end()) {
                frontier.push(next);
                this->cameFrom[next] = current;
                this->grid[next.y][next.x].tileState = TileState::visited;
                // TODO: insert sleep thread
            }
        }
    }

}
