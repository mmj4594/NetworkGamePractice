#include "Player.h"

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
