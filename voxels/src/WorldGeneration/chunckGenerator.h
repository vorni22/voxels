#ifndef CHUNCKGENERATOR_H
#define CHUNCKGENERATOR_H

#include <set>
#include <vector>
#include <thread>
#include <glm/glm.hpp>

#include <algorithm>

#include "chunck.h"

const int RENDER_DIST = 8;
const float BLOCK_SIZE = 0.25f;

struct RenderingBuffers{
	unsigned int VBO, VAO1, VAO2;
};

class ChunckGenerator {
public:
	static void init(int _renderDist = RENDER_DIST, float _blockSize = BLOCK_SIZE);
	
	static void delAll();

	static void Draw(Shader* shader, Camera* camera, bool newPerspective);

	static void changeRenderDistance(int newRenderDist);

	static void update();
	static void updatePos(glm::vec3 newPos);

	static glm::ivec3 getPos();

	static unsigned int get_optimal_threads();

private:
	static void GenerateChunck(int x, int y, int z, int _bufferId);

	static int getBuffer();
	static void reuseBuffer(int i);

};

#endif
