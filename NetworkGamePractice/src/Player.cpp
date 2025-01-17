#include "Player.h"
#include "Net.h"
#include "Block.h"
#include "Game.h"

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
}

void Player::updatePosition(float elapsedTime)
{
	__super::updatePosition(elapsedTime);

	if (getPosition().x < leftBoundary)
	{
		setPosition(glm::vec2(leftBoundary, getPosition().y));
		setAcc(glm::vec2(0, getAcc().y));
	}
	if (getPosition().x > rightBoundary)
	{
		setPosition(glm::vec2(rightBoundary, getPosition().y));

		setAcc(glm::vec2(0, getAcc().y));
	}
	if (getPosition().y <= bottomBoundary)
		setPosition(glm::vec2(getPosition().x, bottomBoundary));

	jumping = getPosition().y - getHeight() / 2 > Game::Get().floor.getPosition().y + Game::Get().floor.getHeight() / 2;
}

void Player::updateSpeed(float elapsedTime)
{
	setSpeed(getSpeed() + (getAcc() * elapsedTime));

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

	// max/min speed
	if (getSpeed().x < getMinSpeed().x) setSpeed(glm::vec2(getMinSpeed().x, getSpeed().y));
	if (getSpeed().y < getMinSpeed().y) setSpeed(glm::vec2(getSpeed().x, getMinSpeed().y));
	if (getSpeed().x > getMaxSpeed().x) setSpeed(glm::vec2(getMaxSpeed().x, getSpeed().y));
	if (getSpeed().y > getMaxSpeed().y) setSpeed(glm::vec2(getSpeed().x, getMaxSpeed().y));

	// handle on land
	if (!jumping)
		setSpeed(glm::vec2(getSpeed().x, 0.f));
}

void Player::jump()
{
	addImpulse(glm::vec2(0.f, PLYAER_JUMP_SPEED));
}
