#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
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
	Game::Get().resetRound();

	// Main loop
	while (!Graphics::Get().shouldClose())
	{
		static int fpsCount = 0;
		static double fpsTimer = 0.0;
		static double tickTimer = 0.0;
		static double renderTimer = 0.0;

		static double previousTime = 0.0;
		const double currentTime = glfwGetTime();
		const double elapsedTime = currentTime - previousTime;
		previousTime = currentTime;

		fpsTimer += elapsedTime;
		tickTimer += (elapsedTime * TIME_SCALE);
		renderTimer += elapsedTime;

		while (tickTimer >= FRAME_TIME)
		{
			Game::Get().tick(static_cast<float>(tickTimer));
			tickTimer -= FRAME_TIME;
		}

		while (renderTimer >= FRAME_TIME)
		{
			Graphics::Get().renderFrame(renderTimer);
			fpsCount++;
			renderTimer -= FRAME_TIME;
		}

		while (fpsTimer >= 1.0)
		{
			printf("FPS: %d\n", fpsCount);
			fpsCount = 0;
			fpsTimer -= 1.0;
		}
	}

	glfwTerminate();
	return 0;
}