#pragma once

#include "GameState.h"

class GameState_Online : public GameState
{
public:
	void beginPlay() override;
	void tick(float elapsedTime) override;
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;
};
