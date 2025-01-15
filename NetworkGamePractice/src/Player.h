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

	void reset() override;

public:
	int getPlayerID() {return playerID;}
	void setPlayerID(int newPlayerID) {playerID = newPlayerID;}

	bool getJumping() {return jumping;}
	void setJumping(bool newJumping) {jumping = newJumping;}

public:
	void moveLeft();
	void moveRight();
	void jump();

private:
	int playerID = 0;
	bool jumping = false;
	float leftBoundary = 0.f;
	float rightBoundary = 0.f;
};
