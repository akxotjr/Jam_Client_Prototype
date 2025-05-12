#pragma once
#include <numbers>


constexpr float WINDOW_SIZE_X = 800;
constexpr float WINDOW_SIZE_Y = 600;

constexpr float WINDOW_CENTER_X = WINDOW_SIZE_X / 2;
constexpr float WINDOW_CENTER_Y = WINDOW_SIZE_Y / 2;

constexpr double LOGIC_TICK_RATE = 30.0;
constexpr double LOGIC_TICK_INTERVAL = 1.0 / LOGIC_TICK_RATE;

//constexpr double RENDER_TICK_RATE = 0.0;
//constexpr double RENDER_TICK_INTERVAL = 1.0 / RENDER_TICK_RATE;

constexpr double SYNC_TIME_INTERVAL = 5.0;

constexpr int32 WORLD_RANGE_MAX = 10000;
constexpr int32 WORLD_RANGE_MIN = -10000;
constexpr int32 WORLD_GRID_SIZE = 10;

constexpr double MIN_INTERPOLATION_DELAY = 0.04;
constexpr double MAX_INTERPOLATION_DELAY = 0.08;

constexpr float YAW_SPEED_MIN = -10.0f;
constexpr float YAW_SPEED_MAX = +10.0f;
constexpr float YAW_SPEED_RANGE = YAW_SPEED_MAX - YAW_SPEED_MIN;

constexpr float DEG2RAD = 3.1415927f / 180.0f;

constexpr float PI = std::numbers::pi_v<float>;

extern bool running;