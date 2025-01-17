#pragma once
#include <glm/glm.hpp>

class GameObject
{
public:
	GameObject() {}
	GameObject(glm::vec2 position_, float width_, float height_) : position(position_), initialPosition(position_), width(width_), height(height_) {}
	virtual void beginPlay() {}

public:
	glm::vec2 getPosition() {return position;}
	void setPosition(glm::vec2 newPosition) {position = newPosition;}

	glm::vec2 getSpeed() {return speed;}
	void setSpeed(glm::vec2 newSpeed) {speed = newSpeed;}
	void addImpulse(glm::vec2 impulse) {speed += impulse;}	
	glm::vec2 getMaxSpeed() {return maxSpeed;}
	void setMaxSpeed(glm::vec2 newMaxSpeed) {maxSpeed = newMaxSpeed;}
	glm::vec2 getMinSpeed() {return minSpeed;}
	void setMinSpeed(glm::vec2 newMinSpeed) {minSpeed = newMinSpeed;}
	
	glm::vec2 getAcc() {return acc;}
	void setAcc(glm::vec2 newAcc) {acc = newAcc;}
	
	float getWidth() {return width;}
	void setWidth(float newWidth) {width = newWidth;}
	
	float getHeight() {return height;}
	void setHeight(float newHeight) {height = newHeight;}
	
	glm::vec3 getColor() {return color;}
	void setColor(glm::vec3 newColor) {color = newColor;}
	
public:
	virtual void reset();
	virtual void updatePosition(float elapsedTime);
	virtual void updateSpeed(float elapsedTime);

private:
	glm::vec2 position = glm::vec2(0.f);
	glm::vec2 initialPosition = glm::vec2(0.f);

	glm::vec2 speed = glm::vec2(0.f);
	glm::vec2 maxSpeed = glm::vec2(0.f, 0.f);
	glm::vec2 minSpeed = glm::vec2(0.f, 0.f);

	glm::vec2 acc = glm::vec2(0.f, 0.f);

	float width = 0.f;
	float height = 0.f;

	glm::vec3 color = glm::vec3(0.f);
};
