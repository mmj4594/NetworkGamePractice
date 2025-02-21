#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <chrono>
#include "Game.h"
#include "Server.h"

float getCurrentTime()
{
	using namespace std::chrono;
	return duration<float>(steady_clock::now().time_since_epoch()).count();
}

int main()
{
	Server::Get().beginPlay();

	Game::Get().beginPlay();

	// Main loop
	while (true)
	{
		static double tickTimer = 0.0;

		static double previousTime = getCurrentTime();
		const double currentTime = getCurrentTime();
		const double elapsedTime = currentTime - previousTime;
		previousTime = currentTime;

		tickTimer += (elapsedTime * Game::Get().currentTimeScale);

		while (tickTimer >= FRAME_TIME)
		{
			Game::Get().tick(static_cast<float>(tickTimer));
			if(Server::Get().getConnectedPlayers() == MAX_PLAYERS)
			{
				Server::Get().tick(static_cast<float>(tickTimer));
			}
			tickTimer -= FRAME_TIME;
		}
	}

	Game::Get().endPlay();

	Server::Get().endPlay();

	return 0;
}
