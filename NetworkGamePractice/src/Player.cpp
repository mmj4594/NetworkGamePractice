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
	if(playerID == 1)
	{
		leftBoundary = Game::Get().leftWall.getPosition().x + Game::Get().leftWall.getWidth() / 2 + getWidth() / 2;
		rightBoundary = Game::Get().net.getPosition().x - Game::Get().net.getWidth() / 2 - getWidth() / 2;
	}
	else
	{
		leftBoundary = Game::Get().net.getPosition().x + Game::Get().net.getWidth() / 2 + getWidth() / 2;
		rightBoundary = Game::Get().rightWall.getPosition().x - Game::Get().rightWall.getWidth() / 2 - getWidth() / 2;
	}
}

void Player::reset()
{
	__super::reset();
	jumping = false;
}

void Player::moveLeft()
{
	setPosition(glm::vec2(getPosition().x - 0.02f, getPosition().y));

	if (getPosition().x <= leftBoundary)
		setPosition(glm::vec2(leftBoundary, getPosition().y));
}

void Player::moveRight()
{
	setPosition(glm::vec2(getPosition().x + 0.02f, getPosition().y));

	if (getPosition().x >= rightBoundary)
		setPosition(glm::vec2(rightBoundary, getPosition().y));
}

void Player::jump()
{
	setSpeed(glm::vec2(getSpeed().x, JUMP_SPEED));
	jumping = true;
}
