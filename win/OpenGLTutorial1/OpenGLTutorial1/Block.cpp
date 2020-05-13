#include "Block.h"
#include <glm/glm.hpp>
#include "Shader.h"

Block::Block(std::vector<Block>* home) : position(0.0f, 0.0f, 0.0f), bt(grass), isSolid(true), destroyed(false) {
	Home = home;
}

Block::Block(glm::vec3 pos, BlockType bt, bool solid, std::vector<Block>* home) : position(pos), bt(bt), isSolid(solid), destroyed(false) {
	position = pos;
	Home = home;
}

void Block::Destroy() {
	//Home.erase();
	//delete this;
}