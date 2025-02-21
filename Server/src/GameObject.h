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
	float getRight() {return position.x + width/2;}
	float getLeft() {return position.x - width/2;}
	float getTop() {return position.y + height/2;}
	float getBottom() {return position.y - height/2;}

	glm::vec2 getSpeed() {return speed;}
	virtual void setSpeed(glm::vec2 newSpeed);
	void addImpulse(glm::vec2 impulse) {setSpeed(speed + impulse);}	
	glm::vec2 getSpeedRange() {return speedRange;}
	void setSpeedRange(glm::vec2 newSpeedRange) {speedRange = newSpeedRange;}
	
	glm::vec2 getAcc() {return acc;}
	void setAcc(glm::vec2 newAcc) {acc = newAcc;}
	
	float getWidth() {return width;}
	void setWidth(float newWidth) {width = newWidth;}
	
	float getHeight() {return height;}
	void setHeight(float newHeight) {height = newHeight;}
	
public:
	virtual void reset();
	virtual void updatePosition(float elapsedTime);
	virtual void updateSpeed(float elapsedTime);

private:
	glm::vec2 position = glm::vec2(0.f);
	glm::vec2 initialPosition = glm::vec2(0.f);

	glm::vec2 speed = glm::vec2(0.f);
	glm::vec2 speedRange = glm::vec2(0.f, 0.f);

	glm::vec2 acc = glm::vec2(0.f, 0.f);

	float width = 0.f;
	float height = 0.f;
};
