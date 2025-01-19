#include "Player.h"
#include "Net.h"
#include "Block.h"
#include "Game.h"
#include <iostream>

Player::Player(int playerID_, glm::vec2 position_, float width_, float height_) : GameObject(position_, width_, height_)
{
	playerID = playerID_;
	playerID == 1 ? setColor(P1_COLOR) : setColor(P2_COLOR);
}

void Player::beginPlay()
{
	setMaxSpeed(PLAYER_MAX_SPEED);
	setMinSpeed(PLAYER_MIN_SPEED);
	setAcc(glm::vec2(0.f, GRAVITY));

	if (playerID == 1)
	{
		leftBoundary = Game::Get().leftWall.getPosition().x + Game::Get().leftWall.getWidth() / 2 + getWidth() / 2;
		rightBoundary = Game::Get().net.getPosition().x - Game::Get().net.getWidth() / 2 - getWidth() / 2;
	}
	else
	{
		leftBoundary = Game::Get().net.getPosition().x + Game::Get().net.getWidth() / 2 + getWidth() / 2;
		rightBoundary = Game::Get().rightWall.getPosition().x - Game::Get().rightWall.getWidth() / 2 - getWidth() / 2;
	}
	bottomBoundary = Game::Get().floor.getPosition().y + Game::Get().floor.getHeight() / 2 + getHeight() / 2;
}

void Player::reset()
{
	__super::reset();

	jumping = true;
	sliding = false;
}

void Player::updatePosition(float elapsedTime)
{
	__super::updatePosition(elapsedTime);

	if (getPosition().x < leftBoundary)
	{
		setPosition(glm::vec2(leftBoundary, getPosition().y));
		setSpeed(glm::vec2(0, getSpeed().y));
		setAcc(glm::vec2(0, getAcc().y));
	}
	if (getPosition().x > rightBoundary)
	{
		setPosition(glm::vec2(rightBoundary, getPosition().y));
		setSpeed(glm::vec2(0, getSpeed().y));
		setAcc(glm::vec2(0, getAcc().y));
	}
	if (getPosition().y <= bottomBoundary)
		setPosition(glm::vec2(getPosition().x, bottomBoundary));
}

void Player::updateSpeed(float elapsedTime)
{
	__super::updateSpeed(elapsedTime);

	// friction
	if (getAcc().x == 0.f)
	{
		glm::vec2 tempSpeed = getSpeed();
		if (tempSpeed.x > 0.f)
		{
			tempSpeed.x -= PLAYER_FRICTION * elapsedTime;
			if (tempSpeed.x < 0.f) tempSpeed.x = 0.f;
		}
		else if (tempSpeed.x < 0.f)
		{
			tempSpeed.x += PLAYER_FRICTION * elapsedTime;
			if (tempSpeed.x > 0.f) tempSpeed.x = 0.f;
		}
		setSpeed(tempSpeed);
	}

	// handle on land
	if (!jumping)
	{
		setSpeed(glm::vec2(getSpeed().x, 0.f));
	}
}

void Player::updateState(float elapsedTime)
{
	if (jumping && getPosition().y <= bottomBoundary)
	{
		jumping = false;
		resetReserveSpike();
	}

	if (sliding)
	{
		slidingTimer += elapsedTime;
		if (slidingTimer >= PLAYER_SLIDING_DURATION)
		{
			sliding = false;
			slidingTimer = 0.0f;
			setMaxSpeed(PLAYER_MAX_SPEED);
			setMinSpeed(PLAYER_MIN_SPEED);
		}
	}
}

void Player::jump()
{
	jumping = true;
	addImpulse(glm::vec2(0.f, PLYAER_JUMP_SPEED));
}

void Player::slide(bool right)
{
	sliding = true;
	slidingTimer = 0.0f;
	setMaxSpeed(PLAYER_SLIDING_MAX_SPEED);
	setMinSpeed(PLAYER_SLIDING_MIN_SPEED);
	setSpeed(glm::vec2(right ? PLAYER_SLIDING_SPEED : -PLAYER_SLIDING_SPEED, 0));
	setAcc(glm::vec2(0, getAcc().y));
}
