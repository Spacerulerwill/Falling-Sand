#include <iostream>
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

		int pixel_index = ONE_PARTICLE * particle_index;

		if (particle->y > 0) {
			// is pixel empty below? 
			if (PIXEL_EMPTY(pixel_index - PARTICLE_PER_ROW)) {
				int velocity = (1 + ACCELERATION * particle->frames);

				if (velocity > particle->y) {
					velocity = particle->y;
				}

				// if so start falling
				for (int i = 1; i <= velocity; i++) {
					if (PIXEL_EMPTY(pixel_index - i * PARTICLE_PER_ROW)) {
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
					if (particle->x > 0 && PIXEL_EMPTY(pixel_index - PARTICLE_PER_ROW - ONE_PARTICLE) && PIXEL_EMPTY(pixel_index-ONE_PARTICLE))
					{
						SwapPixels(particle_index, particle_index - SCREEN_WIDTH - 1);
						particle->x -= 1;
						particle->y -= 1;
						continue;
					}
					// try and go down to rigth
					else if (particle->x < SCREEN_WIDTH - 1 && PIXEL_EMPTY(pixel_index - PARTICLE_PER_ROW + ONE_PARTICLE) && PIXEL_EMPTY(pixel_index+ONE_PARTICLE))
					{
						SwapPixels(particle_index, particle_index - SCREEN_WIDTH + 1);
						particle->x += 1;
						particle->y -= 1;
						continue;
					}

			}
		}
	}
}

void ParticleBuffer::CreateParticle(int x, int y)
{
	int particle_index = PARTICLE_INDEX(x, y);

	if (PIXEL_EMPTY(particle_index * ONE_PARTICLE)) {

		pixels[particle_index * ONE_PARTICLE] = 255;
		pixels[particle_index * ONE_PARTICLE + 1] = 255;
		pixels[particle_index * ONE_PARTICLE + 2] = 255;
		pixels[particle_index * ONE_PARTICLE + 3] = 255;

		particles[particle_index] = new Particle{x, y};
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
