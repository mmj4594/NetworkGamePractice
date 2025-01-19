#pragma once

#include <glm/glm.hpp>
#include "GameObject.h"

const glm::vec3 P1_COLOR = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 P2_COLOR = glm::vec3(0.0f, 0.0f, 1.0f);

enum class SpikeDirection
{
	None,
	Front,
	Up,
	Down,
	Front_Up,
	Front_Down,
};

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

	void reserveSpike() {spikeReserved = true;}
	void resetReserveSpike() {spikeReserved = false;}
	bool getSpikeReserved() {return spikeReserved;}
	void setSpikeDirection(SpikeDirection direction) {spikeDirection = direction;}
	SpikeDirection getSpikeDirection() {return spikeDirection;}

private:
	int playerID = 0;

	bool jumping = false;
	bool sliding = false;
	float slidingTimer = 0.0f;
	bool spikeReserved = false;
	SpikeDirection spikeDirection = SpikeDirection::None;

	float leftBoundary = 0.f;
	float rightBoundary = 0.f;
	float bottomBoundary = 0.f;
};
