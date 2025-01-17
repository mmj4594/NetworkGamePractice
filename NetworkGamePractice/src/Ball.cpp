#include "Ball.h"
#include "Game.h"

Ball::Ball(glm::vec2 position_, float width_, float height_) : GameObject(position_, width_, height_)
{
	setColor(BALL_COLOR);
}

void Ball::beginPlay()
{
	setMaxSpeed(BALL_MAX_SPEED);
	setMinSpeed(BALL_MIN_SPEED);
	setAcc(glm::vec2(0.f, GRAVITY));
}
