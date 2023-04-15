#include "Application.h"
#include <iostream>
#include <sstream>
#include <random>
#include <vector>

#include <vertex/IndexBuffer.h>
#include <vertex/VertexArray.h>
#include <vertex/VertexBuffer.h>
#include <vertex/VertexBufferLayout.h>

#include <core/Window.h>

#define PARTICLE_INDEX(x,y) (x + (y * SCREEN_WIDTH)) * ONE_PARTICLE

// static variable definitions
std::unique_ptr<Application> Application::s_Instance = nullptr;
GLFWwindow* Application::p_Window = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// information for quad that we render the fractal onto
float vertices[] = {
		 1.0f, 1.0f, 1.0f, 1.0f,   // top right
		 1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
		 -1.0f, -1.0f, 0.0f, 0.0f,   // bottom left
		 -1.0f, 1.0f, 0.0f, 1.0f  // top left 
};
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,  // first Triangle
	1, 2, 3   // second Triangle
};

Application::Application()
{
    
}

void Application::Run()
{
	srand(static_cast<unsigned int> (time(NULL)));

	if (!glfwInit()) {
		std::cout << "Failed to intialise GLFW! Aborting..." << std::endl;
		std::exit(-1);
	}

	// initialise window
	// ----------------
	Window::Init("Falling Sand", SCREEN_WIDTH, SCREEN_HEIGHT);
	p_Window = Window::GetWindow();

	glfwSetFramebufferSizeCallback(p_Window, framebuffer_size_callback);

	// initialise GLAD
	// ---------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD! Aborting..." << std::endl;
		std::exit(-1);
	}

	// create buffers for rendering the quad
	VertexBufferLayout layout;
	layout.AddAttribute<float>(2);
	layout.AddAttribute<float>(2);

	VertexBuffer VBO(vertices, sizeof(vertices));
	IndexBuffer EBO(indices, sizeof(indices));

	VertexArray VAO;
	VAO.AddBuffer(VBO, layout);

	VAO.Bind();
	EBO.Bind();

	Shader shader("res/shaders/default.shader");
	shader.Bind();

	// setup pixel buffer
	memset(pixels, 0, sizeof(pixels));

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> x_distr(0, SCREEN_WIDTH-1); // define the range
	std::uniform_int_distribution<> y_distr(0, SCREEN_HEIGHT-1); // define the range

	for (int i = 0; i < 1; i++) {
		int x = x_distr(gen);
		int y = y_distr(gen);
		particles.emplace_back(Particle{ x, y });
		int index = PARTICLE_INDEX(x, y);
		pixels[index] = 255;
		pixels[index + 1] = 255;
		pixels[index + 2] = 255;
		pixels[index + 3] = 255;
	}

	glEnable(GL_TEXTURE_2D);
	// The texture we're going to render to
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// application loop
	// ---------------
	int frames = 0;
	while (!glfwWindowShouldClose(p_Window)) {
		frames++;
		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		/*
		if (frames % 30 == 0){
			particles.emplace_back(Particle{ 400, 400 });
			int index = PARTICLE_INDEX(400, 400);
			pixels[index] = 255;
			pixels[index + 1] = 255;
			pixels[index + 2] = 255;
			pixels[index + 3] = 255;
		}
		*/

		UpdateParticles();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// draw quad to render too - main framebuffer
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (key presses, mouse interactions etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(p_Window);
		glfwPollEvents();
	}
	glfwTerminate();

}

void Application::UpdateParticles()
{
	int particleCount = particles.size();
	for (int i = 0; i < particleCount; i++) {
		Particle* particle = &particles[i];
		
		if (particle->y > 0) {
			int index = PARTICLE_INDEX(particle->x, particle->y);

			std::cout << particle->velocity << std::endl;
			
			// if no particle below go down
			if (pixels[index - PARTICLE_PER_ROW] == 0 &&
				pixels[index - PARTICLE_PER_ROW + 1] == 0 &&
				pixels[index - PARTICLE_PER_ROW + 2] == 0 &&
				pixels[index - PARTICLE_PER_ROW + 3] == 0)
			{
				//go down 
				std::swap(pixels[index], pixels[index - PARTICLE_PER_ROW]);
				std::swap(pixels[index + 1], pixels[index - PARTICLE_PER_ROW + 1]);
				std::swap(pixels[index + 2], pixels[index - PARTICLE_PER_ROW + 2]);
				std::swap(pixels[index + 3], pixels[index - PARTICLE_PER_ROW + 3]);
				particle->y -= 1;
				particle->frames++;
				particle->velocity = ACCELERATION * particle->frames;
				continue;
			}		
			// try and go down and to left
			else if (particle->x > 0 &&
				pixels[index - PARTICLE_PER_ROW - ONE_PARTICLE] == 0 &&
				pixels[index - PARTICLE_PER_ROW - ONE_PARTICLE + 1] == 0 &&
				pixels[index - PARTICLE_PER_ROW - ONE_PARTICLE + 2] == 0 &&
				pixels[index - PARTICLE_PER_ROW - ONE_PARTICLE + 3] == 0) 
			
			{
				std::swap(pixels[index], pixels[index - PARTICLE_PER_ROW - ONE_PARTICLE]);
				std::swap(pixels[index + 1], pixels[index - PARTICLE_PER_ROW - ONE_PARTICLE + 1]);
				std::swap(pixels[index + 2], pixels[index - PARTICLE_PER_ROW - ONE_PARTICLE + 2]);
				std::swap(pixels[index + 3], pixels[index - PARTICLE_PER_ROW - ONE_PARTICLE + 3]);
				particle->x -= 1;
				particle->y -= 1;
				particle->frames++;
				particle->velocity = ACCELERATION * particle->frames;
				continue;
			}
			// try and go down to rigth
			else if (particle->x < SCREEN_WIDTH - 1 &&
				pixels[index - PARTICLE_PER_ROW + ONE_PARTICLE] == 0 &&
				pixels[index - PARTICLE_PER_ROW + ONE_PARTICLE + 1] == 0 &&
				pixels[index - PARTICLE_PER_ROW + ONE_PARTICLE + 2] == 0 &&
				pixels[index - PARTICLE_PER_ROW + ONE_PARTICLE + 3] == 0)

			{
				std::swap(pixels[index], pixels[index - PARTICLE_PER_ROW + ONE_PARTICLE]);
				std::swap(pixels[index + 1], pixels[index - PARTICLE_PER_ROW + ONE_PARTICLE + 1]);
				std::swap(pixels[index + 2], pixels[index - PARTICLE_PER_ROW + ONE_PARTICLE + 2]);
				std::swap(pixels[index + 3], pixels[index - PARTICLE_PER_ROW + ONE_PARTICLE + 3]);
				particle->x += 1;
				particle->frames++;
				particle->velocity = ACCELERATION * particle->frames;
				particle->y -= 1;
				continue;
			}
		}	
		// particle landed - reset frames and velocity
		particle->frames = 0;
		particle->velocity = 0;
		std::cout << particle->velocity << std::endl;
	}
}


std::unique_ptr<Application>& Application::GetInstance() {
    if (Application::s_Instance == nullptr) {
        Application::s_Instance = std::unique_ptr<Application>(new Application);
    }
    return Application::s_Instance;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}