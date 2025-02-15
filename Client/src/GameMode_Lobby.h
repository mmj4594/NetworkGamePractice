#pragma once

#include <glm/glm.hpp>
#include "GameMode.h"

constexpr float SELECTED_GAMEMODE_SIZE = 1.0f;
constexpr float UNSELECTED_GAMEMODE_SIZE = 0.8f;
constexpr glm::vec3 SELECTED_GAMEMODE_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 UNSELECTED_GAMEMODE_COLOR = glm::vec3(0.7f, 0.7f, 0.7f);

class GameMode_Lobby : public GameMode
{
public:
	void beginPlay() override;
	void endPlay() override;
	void tick(float elapsedTime) override;
	void renderFrame(float elapsedTime) override;
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;

private:
	GameModeType selectedGameMode = GameModeType::Local;
};
