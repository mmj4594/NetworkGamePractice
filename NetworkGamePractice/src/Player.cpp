#include "Player.h"
#include "Game.h"

Player::Player(int playerID_, glm::vec2 position_, float width_, float height_) : GameObject(position_, width_, height_)
{
	playerID = playerID_;
	playerID == 1 ? setColor(P1_COLOR) : setColor(P2_COLOR);
}

void Player::reset()
{
	__super::reset();
	jumping = false;
}

void Player::moveLeft()
{
	setPosition(glm::vec2(getPosition().x - 0.02f, getPosition().y));
}

void Player::moveRight()
{
	setPosition(glm::vec2(getPosition().x + 0.02f, getPosition().y));
}

void Player::jump()
{
	setSpeed(glm::vec2(getSpeed().x, JUMP_SPEED));
	jumping = true;
}
