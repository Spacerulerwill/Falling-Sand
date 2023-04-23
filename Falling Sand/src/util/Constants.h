#pragma once

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define PIXEL_COUNT SCREEN_WIDTH * SCREEN_HEIGHT
#define ONE_PARTICLE 4
#define CHANNEL_COUNT PIXEL_COUNT * ONE_PARTICLE
#define PARTICLE_PER_ROW SCREEN_WIDTH * ONE_PARTICLE
#define PARTICLE_PER_COLUMN SCREEN_HEIGHT * ONE_PARTICLE
#define ACCELERATION 0.4f
#define PI 3.1415926536
#define TAU 2 * PI