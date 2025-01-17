#pragma once
#include "GameObject.h"
#include <glm/glm.hpp>

const glm::vec3 P1_COLOR = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 P2_COLOR = glm::vec3(0.0f, 0.0f, 1.0f);

class Player : public GameObject
{
public:
	Player() {}
	Player(int playerID_, glm::vec2 position_, float width_, float height_);
	void beginPlay() override;

public:
	void reset() override;
	void updatePosition(float elapsedTime) override;
	void updateSpeed(float elapsedTime) override;

public:
	void updateState(float elapsedTime);

	int getPlayerID() {return playerID;}
	void setPlayerID(int newPlayerID) {playerID = newPlayerID;}

	void jump();
	bool getJumping() {return jumping;}

	void slide(bool right);
	bool getSliding() {return sliding;}

private:
	int playerID = 0;

	bool jumping = false;
	bool sliding = false;
	float slidingTimer = 0.0f;

	float leftBoundary = 0.f;
	float rightBoundary = 0.f;
	float bottomBoundary = 0.f;
};
