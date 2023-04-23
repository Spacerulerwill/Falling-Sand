#include <iostream>
#include "ParticleBuffer.h"

ParticleBuffer::ParticleBuffer()
{
	// setup pixel buffer
	memset(pixels, 0, sizeof(pixels));
}


void ParticleBuffer::UpdateParticles()
{
	int particleCount = particles.size();
	for (int i = 0; i < particleCount; i++) {
		Particle* particle = &particles[i];

		if (particle->y > 0) {
			int index = PARTICLE_INDEX(particle->x, particle->y);
			int velocity = (1 + ACCELERATION * particle->frames);

			if (velocity > particle->y) {
				velocity = particle->y;
			}

			// is pixel empty below?
			if (PIXEL_EMPTY(index - PARTICLE_PER_ROW)) {
				// if so start falling
				for (int i = 1; i <= velocity; i++) {
					if (PIXEL_EMPTY(index - i * PARTICLE_PER_ROW)) {
						particle->y--;
					}
					else {
						particle->frames = -1;
						break;
					}
				}
				int new_index = PARTICLE_INDEX(particle->x, particle->y);
				SwapPixels(index, new_index);
				particle->frames++;
			}
			else {
				// otherwise try and move to left or right of pixel
				// try and go down and to left
				if (particle->x > 0 && PIXEL_EMPTY(index - PARTICLE_PER_ROW - ONE_PARTICLE))
				{
					SwapPixels(index, index - PARTICLE_PER_ROW - ONE_PARTICLE);
					particle->x -= 1;
					particle->y -= 1;
					continue;
				}
				// try and go down to rigth
				else if (particle->x < SCREEN_WIDTH - 1 && PIXEL_EMPTY(index - PARTICLE_PER_ROW + ONE_PARTICLE))
				{
					SwapPixels(index, index - PARTICLE_PER_ROW + ONE_PARTICLE);
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
	int index = PARTICLE_INDEX(x, y);

	if (PIXEL_EMPTY(index)) {

		pixels[index] = 255;
		pixels[index + 1] = 255;
		pixels[index + 2] = 255;
		pixels[index + 3] = 255;

		particles.emplace_back(Particle{ x, y});
	}
}

void ParticleBuffer::SwapPixels(int index1, int index2)
{
	std::swap(pixels[index1], pixels[index2]);
	std::swap(pixels[index1 + 1], pixels[index2 + 1]);
	std::swap(pixels[index1 + 2], pixels[index2 + 2]);
	std::swap(pixels[index1 + 3], pixels[index2 + 3]);
}
