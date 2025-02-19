#pragma once

#include <glm/glm.hpp>
#include "SharedData.h"
#include "GameMode.h"

constexpr glm::vec3 FPS_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);

struct GLFWwindow;

class GameModeManager
{
public:
	static GameModeManager& Get();

public:
	void beginPlay();
	void endPlay();
	void tick(float elapsedTime);
	void renderFrame(float elapsedTime);
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	void changeGameMode(GameModeType newGameMode);

public:
	GameMode* currentGameMode = nullptr;
	GameModeType currentGameModeType = GameModeType::None;
	float currentTimeScale = BASIC_TIME_SCALE;
};
