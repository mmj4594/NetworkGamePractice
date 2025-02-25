#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <chrono>
#include "SharedData.h"
#include "Game.h"
#include "Server.h"

LogCategory LogMain("Main");

BOOL WINAPI consoleHandler(DWORD signal)
{
	switch (signal)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		Server::Get().reserveShutdown();
		Sleep(1000);
		break;
	}

	return TRUE;
}

float getCurrentTime()
{
	using namespace std::chrono;
	return duration<float>(steady_clock::now().time_since_epoch()).count();
}

int main()
{
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
	{
		LOG(LogMain, LogVerbosity::Error, "Failed to set Console Control Handler!");
		return 1;
	}

	Server::Get().beginPlay();

	// Main loop
	while (!Server::Get().shouldClose())
	{
		static double gameTickTimer = 0.0;
		static double serverTickTimer = 0.0;

		static double previousTime = getCurrentTime();
		const double currentTime = getCurrentTime();
		const double elapsedTime = currentTime - previousTime;
		previousTime = currentTime;

		gameTickTimer += (elapsedTime * Game::Get().currentTimeScale);
		serverTickTimer += elapsedTime * BASIC_TIME_SCALE;

		while (gameTickTimer >= SERVER_FRAME_TIME)
		{
			if (Game::Get().currentGameState != GameStateType::None)
			{
				Game::Get().tick(static_cast<float>(gameTickTimer));
			}
			gameTickTimer -= SERVER_FRAME_TIME;
		}

		while (serverTickTimer >= SERVER_FRAME_TIME)
		{
			if (Game::Get().currentGameState != GameStateType::None)
			{
				Server::Get().tick(static_cast<float>(gameTickTimer));
			}
			serverTickTimer -= SERVER_FRAME_TIME;
		}
	}
	LOG(LogMain, LogVerbosity::Log, "EndPlay Called!");
	Game::Get().endPlay();
	Server::Get().endPlay();

	system("pause");

	return 0;
}
