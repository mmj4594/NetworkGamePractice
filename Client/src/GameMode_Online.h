#pragma once

#include <thread>
#include <WinSock2.h>
#include "GameMode.h"

constexpr int BUFFER_SIZE = 1024;

class GameMode_Online : public GameMode
{
public:
	void beginPlay() override;
	void endPlay() override;
	void tick(float elapsedTime) override;
	void renderFrame(float elapsedTime) override;
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;

public:
	void receiveMessageFromServer();

private:
	SOCKET clientSocket;
	std::thread receiveMessageThread;
};
