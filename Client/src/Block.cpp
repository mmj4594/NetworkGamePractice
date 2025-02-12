#include "Block.h"
#include "GameState_Local.h"

Block::Block(glm::vec2 position_, float width_, float height_) : GameObject(position_, width_, height_)
{
	setColor(BLOCK_COLOR);
}
