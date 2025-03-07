#pragma once

#include "SharedData.h"
#include <glm/glm.hpp>
#include "GameObject.h"

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
	void setSpikeDirection(SpikeDirectionType direction) {spikeDirection = direction;}
	SpikeDirectionType getSpikeDirection() {return spikeDirection;}

private:
	int playerID = 0;

	bool jumping = false;
	bool sliding = false;
	float slidingTimer = 0.0f;
	bool spikeReserved = false;
	SpikeDirectionType spikeDirection = SpikeDirectionType::None;

	float leftBoundary = 0.f;
	float rightBoundary = 0.f;
	float bottomBoundary = 0.f;
};
