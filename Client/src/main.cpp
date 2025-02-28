#include "SharedData.h"
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <chrono>
#include "Graphics.h"
#include "GameModeManager.h"

LogCategory LogMain("Main");

float getCurrentTime()
{
	using namespace std::chrono;
	return duration<float>(steady_clock::now().time_since_epoch()).count();
}

int main()
{
	// Apply Config
	if (!loadConfig("config.json"))
	{
		LOG(LogMain, LogVerbosity::Error, "Failed to load configuration file.");
		return -1;
	}

	if (!Graphics::Get().initializeGraphics())
	{
		LOG(LogMain, LogVerbosity::Error, "Failed to initialize Graphics!");
		return -1;
	}

	LOG(LogMain, LogVerbosity::Log, "Client is Started");
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
		renderTimer += elapsedTime * Config::Get().BASIC_TIME_SCALE;

		while (tickTimer >= Config::Get().CLIENT_FRAME_TIME )
		{
			GameModeManager::Get().tick(static_cast<float>(tickTimer));
			tickTimer -= Config::Get().CLIENT_FRAME_TIME;
		}

		while (renderTimer >= Config::Get().CLIENT_FRAME_TIME )
		{
			Graphics::Get().renderFrame(static_cast<float>(renderTimer));
			renderTimer -= Config::Get().CLIENT_FRAME_TIME;
		}
	}

	GameModeManager::Get().endPlay();

	glfwTerminate();
	LOG(LogMain, LogVerbosity::Log, "Client is Terminated");

	system("pause");

	return 0;
}