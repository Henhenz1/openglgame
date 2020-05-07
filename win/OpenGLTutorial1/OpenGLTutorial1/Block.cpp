#include "Block.h"
#include <glm/glm.hpp>
#include "Shader.h"

Block::Block() : position(0.0f, 0.0f, 0.0f), bt(grass), isSolid(true), destroyed(false) {}

Block::Block(glm::vec3 pos, BlockType bt, bool solid) : position(pos), bt(bt), isSolid(solid), destroyed(false) {}

void Block::Destroy() {
	delete this;
}