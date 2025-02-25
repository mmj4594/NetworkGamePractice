#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <chrono>
#include "Graphics.h"
#include "GameModeManager.h"

float getCurrentTime()
{
	using namespace std::chrono;
	return duration<float>(steady_clock::now().time_since_epoch()).count();
}

int main()
{
	if (Graphics::Get().initializeGraphics() == false)
	{
		std::cerr << "Failed to initialize Graphics!" << std::endl;
		return -1;
	}

	GameModeManager::Get().beginPlay();

	// Main loop
	while (!Graphics::Get().shouldClose())
	{
		static double tickTimer = 0.0;
		static double renderTimer = 0.0;

		static double previousTime = getCurrentTime();
		const double currentTime = getCurrentTime();
		const double elapsedTime = currentTime - previousTime;
		previousTime = currentTime;

		tickTimer += (elapsedTime * GameModeManager::Get().currentTimeScale);
		renderTimer += elapsedTime * BASIC_TIME_SCALE;

		while (tickTimer >= CLIENT_FRAME_TIME )
		{
			GameModeManager::Get().tick(static_cast<float>(tickTimer));
			tickTimer -= CLIENT_FRAME_TIME;
		}

		while (renderTimer >= CLIENT_FRAME_TIME )
		{
			Graphics::Get().renderFrame(static_cast<float>(renderTimer));
			renderTimer -= CLIENT_FRAME_TIME;
		}
	}

	GameModeManager::Get().endPlay();

	glfwTerminate();

	system("pause");

	return 0;
}