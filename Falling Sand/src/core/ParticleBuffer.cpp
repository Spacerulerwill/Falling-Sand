#include <iostream>
#include <random>
#include "ParticleBuffer.h"

ParticleBuffer::ParticleBuffer()
{
	// setup pixel buffer
	memset(pixels, 0, sizeof(pixels));
}


void ParticleBuffer::UpdateParticles()
{
	for (int particle_index = 0; particle_index < PIXEL_COUNT; particle_index++) {
		Particle* particle = particles[particle_index];

		if (particle == nullptr)
			continue;

		switch (particle->type) {
		case SAND: 
			UpdateSand(particle, particle_index);
			break;
		
		}
	}
}

void ParticleBuffer::UpdateSand(Particle* particle, int particle_index)
{
	if (particle->y > 0) {
		// is pixel empty below? 
		if (PIXEL_EMPTY(particle_index - SCREEN_WIDTH)) {
			int velocity = (1 + ACCELERATION * particle->frames);

			if (velocity > particle->y) {
				velocity = particle->y;
			}

			// if so start falling
			for (int i = 1; i <= velocity; i++) {
				if (PIXEL_EMPTY(particle_index - i * SCREEN_WIDTH)) {
					particle->y--;

				}
				else {
					// if collided with a particle
					Particle* particle_below = particles[particle_index - i * SCREEN_WIDTH];
					particle_below->frames = particle->frames;
					break;
				}
			}

			int new_particle_index = PARTICLE_INDEX(particle->x, particle->y);
			SwapPixels(particle_index, new_particle_index);
			particle->frames++;
		}
		else {
			// otherwise try and move to left or right of pixel


			// try and go down and to left
			if (particle->x > 0 && PIXEL_EMPTY(particle_index - SCREEN_WIDTH - 1) && PIXEL_EMPTY(particle_index - 1))
			{
				SwapPixels(particle_index, particle_index - SCREEN_WIDTH - 1);
				particle->x -= 1;
				particle->y -= 1;
				return;
			}
			// try and go down to rigth
			else if (particle->x < SCREEN_WIDTH - 1 && PIXEL_EMPTY(particle_index - SCREEN_WIDTH + 1) && PIXEL_EMPTY(particle_index + 1))
			{
				SwapPixels(particle_index, particle_index - SCREEN_WIDTH + 1);
				particle->x += 1;
				particle->y -= 1;
				return;
			}
			else {
				particle->frames = 0;
			}
		}
	}
}

void ParticleBuffer::CreateParticle(uint8_t type, int x, int y)
{
	int particle_index = PARTICLE_INDEX(x, y);

	RGB rgb = particleColors[type];

	if (PIXEL_EMPTY(particle_index)) {

		pixels[particle_index * ONE_PARTICLE] = rgb.r;
		pixels[particle_index * ONE_PARTICLE + 1] = rgb.g;
		pixels[particle_index * ONE_PARTICLE + 2] = rgb.b;
		pixels[particle_index * ONE_PARTICLE + 3] = 255;

		particles[particle_index] = new Particle{type, x, y, 0};
	}
}

void ParticleBuffer::DeleteParticle(int x, int y)
{
	int particle_index = PARTICLE_INDEX(x, y);
	int pixel_index = particle_index * 4;

	delete particles[particle_index];
	particles[particle_index] = nullptr;
	pixels[pixel_index] = 0;
	pixels[pixel_index+1] = 0;
	pixels[pixel_index+2] = 0;
	pixels[pixel_index+3] = 0;

}

void ParticleBuffer::SwapPixels(int index1, int index2)
{
	std::swap(particles[index1], particles[index2]);
	std::swap(pixels[(index1 * ONE_PARTICLE)], pixels[(index2 * ONE_PARTICLE)]);
	std::swap(pixels[(index1 * ONE_PARTICLE) + 1], pixels[(index2 * ONE_PARTICLE) + 1]);
	std::swap(pixels[(index1 * ONE_PARTICLE) + 2], pixels[(index2 * ONE_PARTICLE) + 2]);
	std::swap(pixels[(index1 * ONE_PARTICLE) + 3], pixels[(index2 * ONE_PARTICLE) + 3]);
}
