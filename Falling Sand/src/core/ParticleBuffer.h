#pragma once

#include <stdint.h>
#include <vector>
#include <util/Constants.h>

#define PARTICLE_INDEX(x,y) (x + ((y) * SCREEN_WIDTH))
#define PIXEL_INDEX(x,y) (PARTICLE_INDEX(x,y)) * ONE_PARTICLE
#define PIXEL_EMPTY(index) (pixels[index] == 0 && pixels[index+1] == 0 && pixels[index+2] == 0 && pixels[index+3] == 0)

struct Particle {
	int x;
	int y;
	int frames;
};

class ParticleBuffer
{
public:
	ParticleBuffer();
	void UpdateParticles();
	void CreateParticle(int x, int y);
	void SwapPixels(int index1, int index2);
	uint8_t pixels[CHANNEL_COUNT];

private:
	Particle* particles[CHANNEL_COUNT] = {};
};
