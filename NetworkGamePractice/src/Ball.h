#pragma once

#include <glm/glm.hpp>
#include "GameObject.h"

class Ball : public GameObject
{
public:
	Ball() {}
	Ball(glm::vec2 position_, float width_, float height_);
	void beginPlay() override;

public:
	void reset() override;

public:
	void setIgnorePlayerCollision(bool newIgnorePlayerCollision) {ignorePlayerCollision = newIgnorePlayerCollision;}
	bool getIgnorePlayerCollision() {return ignorePlayerCollision;}

private:
	bool ignorePlayerCollision = false;
};

