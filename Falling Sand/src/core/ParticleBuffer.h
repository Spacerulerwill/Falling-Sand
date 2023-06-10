#pragma once

#include <stdint.h>
#include <vector>
#include <cstdint>
#include <util/Constants.h>

#define PARTICLE_INDEX(x,y) ((x) + ((y) * SCREEN_WIDTH))
#define PIXEL_INDEX(x,y) (PARTICLE_INDEX(x,y)) * ONE_PARTICLE
#define PIXEL_EMPTY(index) (particles[index] == nullptr)

#define STONE 0
#define SAND 1

struct Particle {
	uint8_t type;
	int x;
	int y;
	int frames;
};

struct RGB {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

class ParticleBuffer
{
public:
	ParticleBuffer();
	void UpdateParticles();
	void UpdateSand(Particle* particle, int particle_index);
	void CreateParticle(uint8_t type, int x, int y);
	void DeleteParticle(int x, int y);
	void SwapPixels(int index1, int index2);
	uint8_t pixels[CHANNEL_COUNT];

private:
	Particle* particles[CHANNEL_COUNT] = {};

	RGB particleColors[2] = {
		{122, 122, 122},
		{205, 170, 109}
	};

};
