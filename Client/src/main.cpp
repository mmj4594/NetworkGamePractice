#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "Graphics.h"
#include "Game.h"

int main()
{
	if (Graphics::Get().initializeGraphics() == false)
	{
		std::cerr << "Failed to initialize Graphics!" << std::endl;
		return -1;
	}

	Game::Get().beginPlay();

	// Main loop
	while (!Graphics::Get().shouldClose())
	{
		static double tickTimer = 0.0;
		static double renderTimer = 0.0;

		static double previousTime = 0.0;
		const double currentTime = glfwGetTime();
		const double elapsedTime = currentTime - previousTime;
		previousTime = currentTime;

		tickTimer += (elapsedTime * Game::Get().currentTimeScale);
		renderTimer += elapsedTime;

		while (tickTimer >= FRAME_TIME)
		{
			Game::Get().tick(static_cast<float>(tickTimer));
			tickTimer -= FRAME_TIME;
		}

		while (renderTimer >= FRAME_TIME)
		{
			Graphics::Get().renderFrame(static_cast<float>(renderTimer));
			renderTimer -= FRAME_TIME;
		}
	}

	glfwTerminate();
	return 0;
}