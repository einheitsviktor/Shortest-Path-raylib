#include "gui.hpp"
#include "fonts.hpp"

#include <raylib.h>

// Constructor
Gui::Gui()
    : search_(Search()),
      mouse_position_({0.0f, 0.0f}),
      origin_state_(TileState::kEmpty),
      start_ptr_(nullptr),
      goal_ptr_(nullptr),
      algorithm_(Algorithm::kBfs),
      start_button_drag_(false),
      goal_button_drag_(false),
      search_executed_(false),
      is_gui_busy_(false),
      is_vector_field_(false),
      grid_(std::vector<std::vector<Tile>>(kMaxTilesY, std::vector<Tile>(kMaxTilesX))) {

    SetTargetFPS(60);
    // Set GUI width and height
    InitWindow(kScreenWidth, kScreenHeight, "Shortest Path raylib");

    // Font generation from TTF font byte array
    // 153616 == hardcoded byte array size of anonymous_pro_bold_ttf
    font_default_.glyphs = LoadFontData(anonymous_pro_bold_ttf, 153616, 24, nullptr, 95, FONT_DEFAULT);
    font_default_.baseSize = 24;
    font_default_.glyphCount = 95;
    Image atlas = GenImageFontAtlas(font_default_.glyphs, &font_default_.recs, 95, 16, 4, 0);
    font_default_.texture = LoadTextureFromImage(atlas);

    // 12136 == hardcoded byte array size of arrows_ttf
    font_unicode_.glyphs = LoadFontData(arrows_ttf, 12136, 24, nullptr, 95, FONT_DEFAULT);
    font_unicode_.baseSize = 24;
    font_unicode_.glyphCount = 95;
    atlas = GenImageFontAtlas(font_unicode_.glyphs, &font_unicode_.recs, 95, 16, 4, 0);
    font_unicode_.texture = LoadTextureFromImage(atlas);
    UnloadImage(atlas);

    // Initialize upper buttons
    const int y = 50;
    int p1 = 40;
    int p2 = p1 + 130;
    int p3 = p2 + 130;
    preset_button1_ = Tile{0, 0, y, p1, 120, 40, "Preset 1"};
    preset_button2_ = Tile{0, 0, y, p2, 120, 40, "Preset 2"};
    preset_button3_ = Tile{0, 0, y, p3, 120, 40, "Preset 3"};

    vector_field_button_ = Tile{10, 13, y, 470, 180, 40, "Vector field"};

    int bfs = 700;
    int djk = bfs + 120;
    int ast = djk + 120;
    bfs_button_ = Tile{10, 40, y, bfs, 110, 40, "Bfs"};
    dijkstra_button_ = Tile{10, 4, y, djk, 110, 40, "Dijkstra"};
    astar_button_ = Tile{10, 23, y, ast, 110, 40, "AStar"};

    clear_button_ = Tile(10, 20, y, 1090, 100, 40, "Clear");
    search_button_ = Tile(10, 25, y, 1200, 120, 40, "Search");

    // Initialize grid
    for (int i = 0, max_tiles = kMaxTilesX * kMaxTilesY; i < max_tiles; ++i) {
        int y = i / (max_tiles / kMaxTilesY);
        int x = i % kMaxTilesX;
        int rec_x = 30.0f + kTileLength * (i % kMaxTilesX) + 1.0f * (i % kMaxTilesX);
        int rec_y = 150.0f + kTileLength * (i / kMaxTilesX) + 1.0f * (i / kMaxTilesX);
        grid_[y][x] = Tile(y, x, rec_y, rec_x, kTileLength, kTileLength, "");
    }
    grid_[3][3].SetTileStart();
    grid_[21][46].SetTileGoal();
    start_ptr_ = &grid_[3][3];
    goal_ptr_ = &grid_[21][46];
}

Gui::~Gui() {
    UnloadFont(font_default_);
    UnloadFont(font_unicode_);
    CloseWindow();
}

void Gui::RunLoop() {
    while (!WindowShouldClose()) {
        ProcessInput();
        GenerateOutput();
    }
}

void Gui::ProcessPresetButton(const Vector2& mouse_pos, Tile* button) {
    if (CheckCollisionPointRec(mouse_pos, button->rec)) {
        button->SetButtonHover();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            button->SetButtonPressed();
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            int start_y, start_x, goal_y, goal_x;
            std::vector<std::vector<int>>* ptr;
            if (button == &preset_button1_) {
                start_y = 14, start_x = 14, goal_y = 1, goal_x = 43;
                ptr = &preset_vec1_;
            } else if (button == &preset_button2_) {
                start_y = 18, start_x = 12, goal_y = 5, goal_x = 27;
                ptr = &preset_vec2_;
            } else {
                start_y = 11, start_x = 14, goal_y = 11, goal_x = 34;
                ptr = &preset_vec3_;
            }
            start_ptr_->SetTileEmpty();
            goal_ptr_->SetTileEmpty();
            PrintPreset(*ptr);
            grid_[start_y][start_x].SetTileStart();
            start_ptr_ = &grid_[start_y][start_x];
            grid_[goal_y][goal_x].SetTileGoal();
            goal_ptr_ = &grid_[goal_y][goal_x];
        } else {
            button->SetButtonHover();
        }
    } else {
        button->SetButtonNormal();
    }
}

void Gui::ProcessAlgorithmButton(const Vector2& mouse_pos, Tile* button) {
    if (CheckCollisionPointRec(mouse_pos, button->rec)) {
        button->SetButtonHover();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            button->SetButtonPressed();
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (button == &bfs_button_) {
                algorithm_ = Algorithm::kBfs;
            } else if (button == &dijkstra_button_) {
                algorithm_ = Algorithm::kDijkstra;
            } else {
                algorithm_ = Algorithm::kAStar;
            }
        } else {
            button->SetButtonHover();
        }
    } else {
        button->SetButtonNormal();
    }
}

void Gui::ProcessActionButton(const Vector2& mouse_pos, Tile* button) {
    if (CheckCollisionPointRec(mouse_pos, button->rec)) {
        button->SetButtonHover();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            button->SetButtonPressed();
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (button == &vector_field_button_) {
                // Toggle vector_field_button_
                is_vector_field_ = !is_vector_field_;
            } else if (button == &clear_button_) {
                ClearGrid();
            } else if (button == &search_button_) {
                if (search_executed_) {
                    PurgeGrid();
                }
                if (algorithm_ == Algorithm::kBfs) {
                    std::thread thread(&Search::Bfs, search_, std::ref(grid_), std::ref(start_ptr_), std::ref(goal_ptr_),
                                       std::ref(is_gui_busy_));
                    thread.detach();
                } else if (algorithm_ == Algorithm::kDijkstra) {
                    std::thread thread(&Search::Dijkstra, search_, std::ref(grid_), std::ref(start_ptr_), std::ref(goal_ptr_),
                                       std::ref(is_gui_busy_));
                    thread.detach();
                } else {
                    std::thread thread(&Search::AStar, search_, std::ref(grid_), std::ref(start_ptr_), std::ref(goal_ptr_),
                                       std::ref(is_gui_busy_));
                    thread.detach();
                }
                search_executed_ = true;
            }
        } else {
            button->SetButtonHover();
        }
    } else {
        button->SetButtonNormal();
    }
}

void Gui::ProcessInput() {
    if (is_gui_busy_) {
        return;
    }
    mouse_position_ = GetMousePosition();

    ProcessPresetButton(mouse_position_, &preset_button1_);
    ProcessPresetButton(mouse_position_, &preset_button2_);
    ProcessPresetButton(mouse_position_, &preset_button3_);

    ProcessActionButton(mouse_position_, &vector_field_button_);

    ProcessAlgorithmButton(mouse_position_, &bfs_button_);
    ProcessAlgorithmButton(mouse_position_, &dijkstra_button_);
    ProcessAlgorithmButton(mouse_position_, &astar_button_);

    ProcessActionButton(mouse_position_, &clear_button_);
    ProcessActionButton(mouse_position_, &search_button_);

    // Process grid
    for (auto& row : grid_) {
        for (auto& tile : row) {
            if (CheckCollisionPointRec(mouse_position_, tile.rec)) {
                // Use left mouse button to place obstacles or drag and drop start and goal
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    if (search_executed_) {
                        PurgeGrid();
                        search_executed_ = false;
                    }
                    if (start_button_drag_) {
                        if (tile.IsTileEmpty()) {
                            tile.SetTileStart();
                            start_ptr_->SetTileEmpty();
                            start_ptr_ = &tile;
                        }
                    } else if (goal_button_drag_) {
                        if (tile.IsTileEmpty()) {
                            tile.SetTileGoal();
                            goal_ptr_->SetTileEmpty();
                            goal_ptr_ = &tile;
                        }
                    } else if (tile.IsTileEmpty()) {
                        tile.SetTileObstacle();
                    } else if (tile.IsTileStart()) {
                        start_button_drag_ = true;
                    } else if (tile.IsTileGoal()) {
                        goal_button_drag_ = true;
                    }
                }
                // Use right mouse button to erase obstacles
                else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    if (tile.IsTileObstacle()) {
                        if (search_executed_) {
                            PurgeGrid();
                            search_executed_ = false;
                        }
                        tile.SetTileEmpty();
                    }
                }
            } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                start_button_drag_ = false;
                goal_button_drag_ = false;
            }
        }
    }
}

void Gui::GeneratePresetButton(const Vector2& mouse_pos, const Tile* button) {
    DrawRectangleRec(button->rec, Fade(BEIGE, 0.4f));
    DrawTextEx(font_default_, button->text.c_str(), Vector2{button->rec.x + 10, button->rec.y + 10}, button->font_size, 0, BLACK);
    if (button->IsButtonPressed()) {
        DrawRectangleRec(button->rec, Fade(BEIGE, 1.0f));
        DrawTextEx(font_default_, button->text.c_str(), Vector2{button->rec.x + 10, button->rec.y + 10}, button->font_size, 0,
                   BLACK);
    } else if (button->IsButtonHover()) {
        DrawRectangleRec(button->rec, Fade(BEIGE, 0.5f));
        DrawTextEx(font_default_, button->text.c_str(), Vector2{button->rec.x + 10, button->rec.y + 10}, button->font_size, 0,
                   BLACK);
    }
}

void Gui::GenerateAlgorithmButton(const Vector2& mouse_pos, const Tile* button) {
    DrawRectangleRec(button->rec, Fade(VIOLET, 0.2f));
    DrawTextEx(font_default_, button->text.c_str(), Vector2{button->rec.x + button->x, button->rec.y + button->y},
               button->font_size, 0, BLACK);
    if (button->IsButtonPressed() || button->IsButtonHover()) {
        DrawRectangleRec(button->rec, Fade(VIOLET, 0.4f));
    }
}

void Gui::GenerateActionButton(const Vector2& mouse_pos, const Tile* button, Color color) {
    DrawRectangleRec(button->rec, Fade(color, 0.4f));
    DrawTextEx(font_default_, button->text.c_str(), Vector2{button->rec.x + button->x, button->rec.y + button->y},
               button->font_size, 0, BLACK);
    if (button->IsButtonPressed()) {
        DrawRectangleRec(button->rec, Fade(color, 1.0f));
        DrawTextEx(font_default_, button->text.c_str(), Vector2{button->rec.x + button->x, button->rec.y + button->y},
                   button->font_size, 0, BLACK);
    } else if (button->IsButtonHover()) {
        DrawRectangleRec(button->rec, Fade(color, 0.5f));
        DrawTextEx(font_default_, button->text.c_str(), Vector2{button->rec.x + button->x, button->rec.y + button->y},
                   button->font_size, 0, BLACK);
    }
}

void Gui::OutlineAlgorithmButton() {
    if (algorithm_ == Algorithm::kBfs) {
        DrawRectangleLinesEx(bfs_button_.rec, 2.0f, BLACK);
    } else if (algorithm_ == Algorithm::kDijkstra) {
        DrawRectangleLinesEx(dijkstra_button_.rec, 2.0f, BLACK);
    } else {
        DrawRectangleLinesEx(astar_button_.rec, 2.0f, BLACK);
    }
}

void Gui::GenerateOutput() {
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        GeneratePresetButton(mouse_position_, &preset_button1_);
        GeneratePresetButton(mouse_position_, &preset_button2_);
        GeneratePresetButton(mouse_position_, &preset_button3_);

        GenerateActionButton(mouse_position_, &vector_field_button_, DARKBLUE);
        if (is_vector_field_) {
            DrawRectangleLinesEx(vector_field_button_.rec, 3.0f, GOLD);
        }

        GenerateAlgorithmButton(mouse_position_, &bfs_button_);
        GenerateAlgorithmButton(mouse_position_, &dijkstra_button_);
        GenerateAlgorithmButton(mouse_position_, &astar_button_);
        OutlineAlgorithmButton();

        GenerateActionButton(mouse_position_, &clear_button_, SKYBLUE);
        GenerateActionButton(mouse_position_, &search_button_, DARKGREEN);

        int offset;
        for (const auto& row : grid_) {
            for (const auto& tile : row) {
                if (tile.IsTileObstacle()) {
                    DrawRectangleRec(tile.rec, Fade(BLACK, 1.0f));
                } else if (tile.IsTileStart()) {
                    DrawRectangleRec(tile.rec, DARKGREEN);
                    DrawTextEx(font_default_, "S", Vector2{tile.rec.x + 7, tile.rec.y + 2}, tile.font_size, 0, RAYWHITE);
                } else if (tile.IsTileGoal()) {
                    DrawRectangleRec(tile.rec, RED);
                    DrawTextEx(font_default_, "G", Vector2{tile.rec.x + 7, tile.rec.y + 2}, tile.font_size, 0, RAYWHITE);
                } else if (tile.IsTileVisited()) {
                    DrawRectangleRec(tile.rec, Fade(DARKBLUE, 0.3f));
                    offset = (tile.text == "C" || tile.text == "D") ? 8 : 3;
                    DrawTextEx(font_unicode_, is_vector_field_ ? tile.text.c_str() : "",
                               Vector2{tile.rec.x + offset, tile.rec.y + 4}, tile.font_size, 0, RAYWHITE);
                } else if (tile.IsTilePath()) {
                    DrawRectangleRec(tile.rec, Fade(GOLD, 0.5f));
                } else {
                    DrawRectangleRec(tile.rec, Fade(LIGHTGRAY, 0.5f));
                }
            }
        }
    }
    EndDrawing();
}

void Gui::ClearGrid() {
    for (auto& row : grid_) {
        for (auto& tile : row) {
            if (!tile.IsTileStart() && !tile.IsTileGoal()) {
                tile.text = "";
                tile.SetTileEmpty();
            }
        }
    }
    search_executed_ = false;
}

void Gui::PurgeGrid() {
    for (auto& row : grid_) {
        for (auto& tile : row) {
            if (tile.IsTilePath() || tile.IsTileVisited()) {
                tile.text = "";
                tile.SetTileEmpty();
            }
        }
    }
    search_executed_ = false;
}

void Gui::PrintPreset(const std::vector<std::vector<int>>& vec) {
    ClearGrid();
    for (const auto& v : vec) {
        grid_[v[0]][v[1]].SetTileObstacle();
    }
}
