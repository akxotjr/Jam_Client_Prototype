#pragma once

constexpr int32 WINDOW_SIZE_X = 800;
constexpr int32 WINDOW_SIZE_Y = 600;

constexpr double LOGIC_TICK_RATE = 30.0;
constexpr double LOGIC_TICK_INTERVAL = 1.0 / LOGIC_TICK_RATE;

//constexpr double RENDER_TICK_RATE = 0.0;
//constexpr double RENDER_TICK_INTERVAL = 1.0 / RENDER_TICK_RATE;

constexpr double SYNC_TIME_INTERVAL = 5.0;

constexpr int32 WORLD_RANGE_MAX = 10000;
constexpr int32 WORLD_RANGE_MIN = -10000;
constexpr int32 WORLD_GRID_SIZE = 10;