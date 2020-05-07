#ifndef BLOCK_H
#define BLOCK_H

#include <glad/glad.h>
#include <glm/glm.hpp>

enum BlockType {
	grass = 0,
	water = 1,
	red = 2,
	yellow = 3,
	purple = 4,
	cyan = 5,
	grey = 6,
};

class Block {
public:
	glm::vec3 position;
	BlockType bt;
	bool isSolid;
	bool destroyed;

	// constructor
	Block();
	Block(glm::vec3 pos, BlockType bt, bool solid);

	void Destroy();
};

#endif
