#pragma once

#include <memory>
#include <vector>
#include <stdint.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader/Shader.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define PIXEL_COUNT SCREEN_WIDTH * SCREEN_HEIGHT
#define ONE_PARTICLE 4
#define CHANNEL_COUNT PIXEL_COUNT * ONE_PARTICLE
#define PARTICLE_PER_ROW SCREEN_WIDTH * ONE_PARTICLE
#define PARTICLE_PER_COLUMN SCREEN_HEIGHT * ONE_PARTICLE
#define ACCELERATION 0.01f

struct Particle {
	int x;
	int y;
	int frames;
};

class Application
{
private:
	static std::unique_ptr<Application> s_Instance;
	static GLFWwindow* p_Window;

	uint8_t pixels[CHANNEL_COUNT];
	std::vector<Particle> particles;

	void UpdateParticles();

public:
	Application();

	void Run();

	static std::unique_ptr<Application>& GetInstance();
};

