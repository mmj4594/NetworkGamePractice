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
	void setPosition(glm::vec2 newPosition);

	glm::vec2 getSpeed() {return speed;}
	void setSpeed(glm::vec2 newSpeed) {speed = newSpeed;}
	
	float getWidth() {return width;}
	void setWidth(float newWidth) {width = newWidth;}
	
	float getHeight() {return height;}
	void setHeight(float newHeight) {height = newHeight;}
	
	glm::vec3 getColor() {return color;}
	void setColor(glm::vec3 newColor) {color = newColor;}

	virtual void reset();

private:
	glm::vec2 position = glm::vec2(0.f);
	glm::vec2 initialPosition = glm::vec2(0.f);
	glm::vec2 speed = glm::vec2(0.f);
	float width = 0.f;
	float height = 0.f;
	glm::vec3 color = glm::vec3(0.f);
};
