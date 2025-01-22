#include "GameObject.h"

void GameObject::setSpeed(glm::vec2 newSpeed)
{
	speed = newSpeed;

	// max/min speed
	if (speed.x < speedRange.x) speed.x = speedRange.x;
	if (speed.y < speedRange.x) speed.y = speedRange.x;
	if (speed.x > speedRange.y) speed.x = speedRange.y;
	if (speed.y > speedRange.y) speed.y = speedRange.y;
}

void GameObject::reset()
{
	position = initialPosition;
	speed = glm::vec2(0.f);
}

void GameObject::updatePosition(float elapsedTime)
{
	position += (speed * elapsedTime);
}

void GameObject::updateSpeed(float elapsedTime)
{
	setSpeed(speed + (acc * elapsedTime));
}