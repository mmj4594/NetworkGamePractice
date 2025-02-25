#include "SharedData.h"
#include "GameObject.h"

LogCategory LogGameObject("GameObject");

void GameObject::setSpeed(glm::vec2 newSpeed)
{
	speed = newSpeed;

	// max/min speed
	if (speed.x < speedRange.x) speed.x = speedRange.x;
	if (speed.x > speedRange.y) speed.x = speedRange.y;
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