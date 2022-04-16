#include "GUI.hpp"
#include <cstdio>
#include <raylib.h>
#include <iostream>

GUI::GUI() {}

void GUI::Initialize() {
    // Set GUI fps
    SetTargetFPS(60);
    // Set GUI width and height
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "New GUI");
    // Initialize tile positions in grid
    for (int i = 0, max_tiles = MAX_TILES_X * MAX_TILES_Y; i < max_tiles; ++i) {
        this->grid[i/(max_tiles/MAX_TILES_Y)][i%MAX_TILES_X].rec.x =
            30.0f  + TILE_WIDTH * (i % MAX_TILES_X) + 1.0f * (i % MAX_TILES_X);
        this->grid[i/(max_tiles/MAX_TILES_Y)][i%MAX_TILES_X].rec.y =
            150.0f + TILE_WIDTH * (i / MAX_TILES_X) + 1.0f * (i / MAX_TILES_X);
        this->grid[i/(max_tiles/MAX_TILES_Y)][i%MAX_TILES_X].rec.width = TILE_WIDTH;
        this->grid[i/(max_tiles/MAX_TILES_Y)][i%MAX_TILES_X].rec.height = TILE_HEIGHT;
    }

    //         y  x
    this->grid[3][3].tileState = TileState::start;
    this->grid[21][46].tileState = TileState::goal;
    this->startPtr = &this->grid[3][3];
    this->goalPtr = &this->grid[21][46];

    // Initialize the reset button
    this->clearButton.rec.x = 1040;
    this->clearButton.rec.y = 50;
    this->clearButton.rec.width = 100;
    this->clearButton.rec.height = 40;

    this->searchButton.rec.x = 1150;
    this->searchButton.rec.y = 50;
    this->searchButton.rec.width = 120;
    this->searchButton.rec.height = 40;
}

void GUI::RunLoop() {
    while (!WindowShouldClose()) {
        ProcessInput();
        GenerateOutput();
    }
}

void GUI::Shutdown() {
    CloseWindow();
}

// Inside while loop
void GUI::ProcessInput() {
    this->mousePosition = GetMousePosition();
    // this->btnAction = false;

    for (auto& row : this->grid) {
        for (auto& col : row) {
            if (CheckCollisionPointRec(this->mousePosition, col.rec)) {
                // Use left mouse button to place obstacles or drag and drop start and goal
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    // if (col.buttonState == ButtonState::normal) {
                    if (this->startButtonDrag) {
                        if (col.isEmpty()) {
                            col.tileState = TileState::start;
                            this->startPtr->tileState = TileState::empty;
                            this->startPtr = &col;
                        }
                    }
                    else if (this->goalButtonDrag) {
                        if (col.isEmpty()) {
                            col.tileState = TileState::goal;
                            this->goalPtr->tileState = TileState::empty;
                            this->goalPtr =  &col;
                        }
                    }
                    else if (col.isEmpty()) {
                        col.tileState = TileState::obstacle;
                    }
                    else if (col.isStart()) {
                        this->startButtonDrag = true;
                    }
                    else if (col.isGoal()) {
                        this->goalButtonDrag = true;
                    }

                    // TODO: Convert to if else cases?
                    // switch (col.tileState) {
                    //     case TileState::empty : {
                    //         col.tileState = TileState::obstacle;
                    //         // col.buttonState = ButtonState::pressed;
                    //         break;
                    //     }
                    //     case TileState::obstacle : break;
                    //     case TileState::none : break;
                    //     case TileState::start: {
                    //         this->startButtonDrag = true;
                    //         break;
                    //     }
                    //     case TileState::goal : {
                    //         this->goalButtonDrag = true;
                    //         break;
                    //     }
                    //     // default: col.buttonState = ButtonState::pressed; break;
                    //     default: break;
                    // }

                    // }
                }
                // Use right mouse button to erase obstacles
                else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    if (col.isObstacle()) {
                        col.tileState = TileState::empty;
                        // col.buttonState = ButtonState::pressed;
                    }
                }
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                // col.buttonState = ButtonState::normal;
                this->startButtonDrag = false;
                this->goalButtonDrag = false;
            }
        }
    }

}


void GUI::GenerateOutput() {
    BeginDrawing();
    {
        // Draw control buttons
        DrawRectangleRec(this->clearButton.rec, SKYBLUE);
        DrawText("Clear", this->clearButton.rec.x+20, this->clearButton.rec.y+10, 24, BLACK);

        DrawRectangleRec(this->searchButton.rec, Fade(DARKGREEN, 0.6f));
        DrawText("Search", this->searchButton.rec.x+20, this->searchButton.rec.y+10, 24, BLACK);

        ClearBackground(RAYWHITE);
        for (const auto& row : this->grid) {
            for (const auto& col : row) {
                if (col.isObstacle()) {
                    DrawRectangleRec(col.rec, Fade(BLACK, 0.7f));
                }
                else if (col.isStart()) {
                    DrawRectangleRec(col.rec, GREEN);
                    DrawText("S", col.rec.x+5, col.rec.y+2, 24, DARKBROWN);
                }
                else if (col.isGoal()) {
                    DrawRectangleRec(col.rec, RED);
                    DrawText("G", col.rec.x+5, col.rec.y+2, 24, BLACK);
                }
                else {
                    DrawRectangleRec(col.rec, Fade(LIGHTGRAY, 0.5f));
                }
            }
        }
    }
    EndDrawing();
}
