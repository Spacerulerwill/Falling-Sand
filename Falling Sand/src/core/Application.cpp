#include "Application.h"
#include <iostream>
#include <random>
#include <vector>

#include <vertex/IndexBuffer.h>
#include <vertex/VertexArray.h>
#include <vertex/VertexBuffer.h>
#include <vertex/VertexBufferLayout.h>

#include <core/Window.h>
#include <core/ParticleBuffer.h>

int clip(int n, int upper, int lower);

// static variable definitions
std::unique_ptr<Application> Application::s_Instance = nullptr;
GLFWwindow* Application::p_Window = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


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
	glfwSetMouseButtonCallback(p_Window, mouse_callback);
	glfwSetScrollCallback(p_Window, scroll_callback);
	glfwSetKeyCallback(p_Window, key_callback);

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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, particle_buffer.pixels);
	
	// application loop
	// ---------------
	while (!glfwWindowShouldClose(p_Window)) {
		InputHandle();
		glfwPollEvents();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		particle_buffer.UpdateParticles();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, particle_buffer.pixels);

		// draw quad to render too - main framebuffer
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (key presses, mouse interactions etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(p_Window);
	}
	glfwTerminate();

}

void Application::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lmouse = true;
		else if (GLFW_RELEASE == action)
			lmouse = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			rmouse = true;
		}
		else if (action == GLFW_RELEASE)
			rmouse = false;
	}
}

void Application::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	cursor_radius = clip(cursor_radius + 10 * yoffset, 10, 300);
}


void Application::InputHandle()
{
	
	if (lmouse) {
		double xpos, ypos;
		glfwGetCursorPos(p_Window, &xpos, &ypos);

		int width, height;
		glfwGetWindowSize(p_Window, &width, &height);

		if (particle_type == STONE) {
			for (int i = xpos - cursor_radius; i < xpos + cursor_radius; i++) {
				for (int j = ypos - cursor_radius; j < ypos + cursor_radius; j++) {
					int a = i - xpos;
					int b = j - ypos;
					if (a* a + b * b <= cursor_radius * cursor_radius) {
						int x = clip(a + xpos, 0, SCREEN_WIDTH - 1);
						int y = clip(b + SCREEN_HEIGHT - ypos, 0, SCREEN_HEIGHT - 1);
						particle_buffer.CreateParticle(particle_type, x, y);
					}
				}
			}
		}
		else {
			for (int i = 0; i < cursor_radius * TAU; i++) {
				double r = cursor_radius * sqrt(((double)rand() / (RAND_MAX)));
				double theta = ((double)rand() / (RAND_MAX)) * TAU;
				int offsetX = r * cos(theta);
				int offsetY = r * sin(theta);

				double xpos, ypos;
				glfwGetCursorPos(p_Window, &xpos, &ypos);

				int width, height;
				glfwGetWindowSize(p_Window, &width, &height);

				int x = ((xpos + offsetX) / width) * SCREEN_WIDTH;
				int y = ((height - (ypos + offsetY)) / height) * SCREEN_HEIGHT;

				x = clip(x, 0, SCREEN_WIDTH - 1);
				y = clip(y, 0, SCREEN_HEIGHT - 1);

				particle_buffer.CreateParticle(particle_type, x, y);
			}
		}
	} 

	if (rmouse) {
		double xpos, ypos;
		glfwGetCursorPos(p_Window, &xpos, &ypos);

		int width, height;
		glfwGetWindowSize(p_Window, &width, &height);

		for (int i = xpos - cursor_radius; i < xpos + cursor_radius; i++) {
			for (int j = ypos - cursor_radius; j < ypos + cursor_radius; j++) {
				int a = i - xpos;
				int b = j - ypos;

				if (a * a + b * b <= cursor_radius * cursor_radius) {
					int x = clip(a + xpos, 0, SCREEN_WIDTH - 1);
					int y = clip(b + SCREEN_HEIGHT - ypos, 0, SCREEN_HEIGHT - 1);
					particle_buffer.DeleteParticle(x, y);
				}
			}
		}
	}

}

void Application::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		particle_type = STONE;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		particle_type = SAND;
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
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	Application::GetInstance()->MouseCallback(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Application::GetInstance()->ScrollCallback(window, xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application::GetInstance()->KeyCallback(window, key, scancode, action, mods);
}

int clip(int n, int lower, int upper) {
	return std::max(lower, std::min(n, upper));
}