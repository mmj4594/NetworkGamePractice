#include "Ball.h"
#include "Game.h"

Ball::Ball(glm::vec2 position_, float width_, float height_) : GameObject(position_, width_, height_)
{
	setColor(BALL_COLOR);
}

void Ball::beginPlay()
{
	setSpeedRange(BALL_SPEED_RANGE);
	setAcc(glm::vec2(0.f, GRAVITY));
}

void Ball::reset()
{
	__super::reset();
	setPosition(glm::vec2((rand() % 2 == 0 ? getInitialPosition().x : SCREEN_WIDTH - getInitialPosition().x), getInitialPosition().y));
}
