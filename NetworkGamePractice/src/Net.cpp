#include "Net.h"

Net::Net(glm::vec2 position_, float width_, float height_) : GameObject(position_, width_, height_)
{
	setColor(NET_COLOR);
}
