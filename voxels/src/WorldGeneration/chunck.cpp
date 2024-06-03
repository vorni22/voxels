#include "chunck.h"

glm::vec3 blockColor[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 1.0f)
};

glm::vec3 faceUpVertices[] = {
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f)
};
glm::vec3 faceBottomVertices[] = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f)
};

glm::vec3 faceFrontVertices[] = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f)
};
glm::vec3 faceBackVertices[] = {
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f)
};

glm::vec3 faceLeftVertices[] = {
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f)
};
glm::vec3 faceRightVertices[] = {
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f)
};

Chunck::Chunck(int locx, int locy, int locz, 
			   float _blockSize, 
			   std::string seed, 
			   chunck_buffer* _buffer, 
			   int buffersID, 
		       void (*_callBack)(int i),
			   vlibs::homogeneous_thread_pool<Chunck*> *_thread_pool)
{
	if (_blockSize < 0.1f) _blockSize = 0.1f;
	if (_blockSize > 2.0f) _blockSize = 2.0f;

	loaded = false;
	initialized = false;

	bufferID = buffersID;
	chunckSeed = seed;
	chunckX = locx;
	chunckY = locy;
	chunckZ = locz;
	lstId = 0;
	callBack = _callBack;
	blockSize = _blockSize;

	buffer = _buffer;

	thread_pool = _thread_pool;

	thread_done = false;
	thread_pool->push_task(&Chunck::thread_work, this);
}

Chunck::~Chunck(){
	callBack(bufferID);

	delete[] blocks;

	// will save vertices first in the fututre

	faces_opaque.clear();
	faces_transparent.clear();
}

// CALL THIS IN THE MAIN THREAD ONLY WHEN NOTIFIED BY THE THREAD POOL!
void Chunck::init(){
	if (initialized) return;

	if (thread_done) {

		buffer->assign(faces_opaque, faces_transparent);
		
		loaded = true;
		initialized = true;
	}
}

Chunck* Chunck::thread_work(){

	noise.CONTINENTAL.SetSeed(0);
	noise.EROZION.SetSeed(1);
	noise.RELIEF.SetSeed(2);

	noise.CONTINENTAL.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	noise.EROZION.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	noise.RELIEF.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);

	blocks = new uint8_t[xLen * yLen * zLen];

	// Get blocks and construct vertices
	for (int y = 0; y < yLen; y++) {
		for (int z = 0; z < zLen; z++) {
			for (int x = 0; x < xLen; x++) {
				int blockx = xLen * chunckX + x;
				int blockz = zLen * chunckZ + z;
				int blocky = yLen * chunckY + y;
				setBlock(x, y, z, getBlock(blockx, blocky, blockz));

				if (y) {
					constructBlockFaces(x, y - 1, z);
				}
			}
		}
	}

	thread_done = true;

	return this;
}

BlockType Chunck::getBlock(int x, int y, int z){
	// replace this with any function that determine the type of the block (x, y, z)
	float continental = (noise.CONTINENTAL.GetNoise((float)x / 16.0f, (float)z / 16.0f));
	float erozion = (noise.EROZION.GetNoise((float)x / 5.0f, (float)z / 5.0f));
	float peaks = (noise.RELIEF.GetNoise((float)x / 2.0f, (float)z / 2.0f));

	float yLim = 1.0f + get_continental(continental) * (1.0f + get_erozion(erozion) * (1.0f + get_peaks(peaks))) * 30.0f;

	if (y <= yLim) return SOLID;
	else if (y <= 60) return WATER;
	else return AIR;
}

inline float Chunck::get_continental(float x){
	if (x <= -0.2f) {
		return 0.1f;
	}

	if (x <= -0.1f) {
		return parabolic_interpolation(x, -0.2f, 0.1f, -0.1f, 0.5f);
	}

	if (x <= 0.0f) {
		return 0.5f;
	}

	if (x <= 0.1f) {
		return parabolic_interpolation(x, 0.0f, 0.5f, 0.1f, 1.0f);
	}

	if (x <= 0.5f) {
		return 1.0f;
	}

	if (x <= 0.6f) {
		return parabolic_interpolation(x, 0.5f, 1.0f, 0.6f, 1.5f);
	}

	if (x <= 0.8f) {
		return 1.5f;
	}

	if (x <= 0.9f) {
		return parabolic_interpolation(x, 0.8f, 1.5f, 0.9f, 2.0f);
	}

	return 2.0f;
}

inline float Chunck::get_erozion(float x){
	if (x <= -0.3f) {
		return 1.0f;
	}

	if (x <= 0.0f) {
		return parabolic_interpolation(x, -0.3f, 1.0f, 0.0f, 0.1f);
	}

	if (x <= 0.3f) {
		return parabolic_interpolation(x, 0.0f, 0.1f, 0.3f, 1.0f);
	}

	if (x <= 0.8f) {
		return 1.0f;
	}

	return parabolic_interpolation(x, 0.8f, 1.0f, 1.0f, 2.0f);
}

inline float Chunck::get_peaks(float x) {
	return parabolic_interpolation(x, -1.0f, 0.0f, 1.0f, 0.5f);
}


inline float Chunck::parabolic_interpolation(float x, float x1, float y1, float x2, float y2){
	float ym = (y1 + y2) / 2.0f;
	float xm = (x1 + x2) / 2.0f;

	if (x <= xm && x >= x1) {
		float a = (ym - y1) / ((x1 - xm) * (x1 - xm));
		float b = -2.0f * a * x1;
		float c = y1 - b * x1 - a * x1 * x1;

		return (a * x * x + b * x + c);
	}
	else if (x > xm && x <= x2) {
		float a = (ym - y2) / ((x2 - xm) * (x2 - xm));
		float b = -2.0f * a * x2;
		float c = y2 - b * x2 - a * x2 * x2;

		return (a * x * x + b * x + c);
	}

	return 0.0f;
}

inline float Chunck::liniar_interpolation(float x, float x1, float y1, float x2, float y2){
	return (((y2 - y1) / (x2 - x1)) * (x - x1) + y1) * (x >= x1 && x <= x2);
}

inline float Chunck::exponential_interpolation(float x, float x1, float y1, float x2, float y2){
	float c = std::pow(std::pow(y2, x1) / pow(y1, x2), 1.0f / (x1 - x2));
	float a = (1.0f / x1) * std::log(y1 / c);
	return c * std::exp(a * x);
}

// CALL THIS IN THE MAIN THREAD ONLY
void Chunck::Draw_opaque(Shader* _shader, Camera* camera, bool newPerspective){

	if (!loaded || !initialized) return;

	_shader->setFloat("blockSize", blockSize);
	_shader->setInt("cx", chunckX);
	_shader->setInt("cy", chunckY);
	_shader->setInt("cz", chunckZ);

	buffer->draw_opaque();
}

void Chunck::Draw_transparent(Shader* _shader, Camera* camera, bool newPerspective){
	if (!loaded || !initialized) return;

	_shader->setFloat("blockSize", blockSize);
	_shader->setInt("cx", chunckX);
	_shader->setInt("cy", chunckY);
	_shader->setInt("cz", chunckZ);

	buffer->draw_transparent();
}

int Chunck::block(int x, int y, int z){
	return blocks[y * xLen * zLen + z * xLen + x];
}

void Chunck::setBlock(int x, int y, int z, BlockType type){
	blocks[y * xLen * zLen + z * xLen + x] = type;
}

bool Chunck::isLoaded() {
	return loaded;
}

bool Chunck::isFailed(){
	return failed;
}

GLuint Chunck::encode(GLuint x, GLuint y, GLuint z, GLuint orientation, GLuint blockType){
	return x + y * xLen + z * xLen * yLen + orientation * xLen * yLen * zLen + blockType * xLen * yLen * zLen * 6;
}

void Chunck::uptadateRelativePos(glm::ivec3 generatorPos){
	relativeX = chunckX - generatorPos.x;
	relativeY = chunckY - generatorPos.y;
	relativeZ = chunckZ - generatorPos.z;
}

glm::ivec3 Chunck::getRelativePos(){
	return glm::ivec3(relativeX, relativeY, relativeZ);
}

glm::vec3 Chunck::getRealRelativePos(glm::vec3 real_generatorPos){
	return glm::vec3(((float)chunckX + 0.5f) * (float)xLen - real_generatorPos.x,
					 ((float)chunckY) * (float)yLen - real_generatorPos.y,
				     ((float)chunckZ + 0.5f) * (float)zLen - real_generatorPos.z
					);
}

bool Chunck::isOnBorder(int x, int y, int z){
	return (x == 0 || z == 0 || x == xLen - 1 || z == zLen - 1);
}

void Chunck::constructBlockFaces(int x, int y, int z){
	// int blockStart = lstId, blockEnd = lstId;

	int block_type = block(x, y, z);
	if (block_type == AIR)
		return;

	// upper face
	if (y == yLen - 1) {
		int neighbour = getBlock(x + chunckX * xLen, y + chunckY * yLen + 1, z + chunckZ * zLen);
		if(neighbour == AIR || (neighbour == WATER && block_type != WATER)) {
			addFace(yUp, x, y, z);
			lstId += 6;
		}
	}
	else {
		if (block(x, y + 1, z) == AIR || (block(x, y + 1, z) == WATER && block_type != WATER)) {
			addFace(yUp, x, y, z);
			lstId += 6;
		}
	}

	// bottom face
	if (y == 0) {
		int neighbour = getBlock(x + chunckX * xLen, y + chunckY * yLen - 1, z + chunckZ * zLen);
		if (neighbour == AIR || (neighbour == WATER && block_type != WATER)) {
			addFace(yBottom, x, y, z);
			lstId += 6;
		}
	}
	else {
		if (block(x, y - 1, z) == AIR || (block(x, y - 1, z) == WATER && block_type != WATER)) {
			addFace(yBottom, x, y, z);
			lstId += 6;
		}
	}

	// front face
	if (z == 0) {
		int neighbour = getBlock(x + chunckX * xLen, y + chunckY * yLen, z + chunckZ * zLen - 1);
		if (neighbour == AIR || (neighbour == WATER && block_type != WATER)) {
			addFace(zFront, x, y, z);
			lstId += 6;
		}
	}
	else {
		if (block(x, y, z - 1) == AIR || (block(x, y, z - 1) == WATER && block_type != WATER)) {
			addFace(zFront, x, y, z);
			lstId += 6;
		}
	}

	// back face
	if (z == zLen - 1) {
		int neighbour = getBlock(x + chunckX * xLen, y + chunckY * yLen, z + chunckZ * zLen + 1);
		if (neighbour == AIR || (neighbour == WATER && block_type != WATER)) {
			addFace(zBack, x, y, z);
			lstId += 6;
		}
	}
	else {
		if (block(x, y, z + 1) == AIR || (block(x, y, z + 1) == WATER && block_type != WATER)) {
			addFace(zBack, x, y, z);
			lstId += 6;
		}
	}

	// left face
	if (x == 0) {
		int neighbour = getBlock(x + chunckX * xLen - 1, y + chunckY * yLen, z + chunckZ * zLen);
		if (neighbour == AIR || (neighbour == WATER && block_type != WATER)) {
			addFace(xLeft, x, y, z);
			lstId += 6;
		}
	}
	else {
		if (block(x - 1, y, z) == AIR || (block(x - 1, y, z) == WATER && block_type != WATER)) {
			addFace(xLeft, x, y, z);
			lstId += 6;
		}
	}

	// right face
	if (x == xLen - 1) {
		int neighbour = getBlock(x + chunckX * xLen + 1, y + chunckY * yLen, z + chunckZ * zLen);
		if (neighbour == AIR || (neighbour == WATER && block_type != WATER)) {
			addFace(xRight, x, y, z);
			lstId += 6;
		}
	}
	else {
		if (block(x + 1, y, z) == AIR || (block(x + 1, y, z) == WATER && block_type != WATER)) {
			addFace(xRight, x, y, z);
			lstId += 6;
		}
	}
}

void Chunck::addFace(Faces face, GLuint x, GLuint y, GLuint z){

	if (block(x, y, z) == WATER)
		faces_transparent.push_back(encode(x, y, z, (GLuint)face, (GLuint)block(x, y, z)));
	else
		faces_opaque.push_back(encode(x, y, z, (GLuint)face, (GLuint)block(x, y, z)));
}
