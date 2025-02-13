#pragma once

#include "GameMode.h"

class GameMode_Online : public GameMode
{
public:
	void beginPlay() override;
	void endPlay() override;
	void tick(float elapsedTime) override;
	void renderFrame(float elapsedTime) override;
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;
};
