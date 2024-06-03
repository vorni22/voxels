#ifndef CHUNCK_H
#define CHUNCK_H

#include <cstdint>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <mutex>

#include "../Graphics/camera.h"
#include "../Graphics/shader.h"

#include "../Noise/FastNoiseLite.h"

#include "../Threads/threadPool.hpp"

#include "../Graphics/buffers.h"

const GLuint xLen = 32, zLen = 32, yLen = 320;

struct NOISE{
	FastNoiseLite CONTINENTAL;
	FastNoiseLite EROZION;
	FastNoiseLite RELIEF;
};

enum BlockType {
	AIR = 0,
	WATER,
	SOLID
};

enum Faces {
	zBack = 0,
	zFront,
	yBottom,
	yUp,
	xRight,
	xLeft
};

class Chunck {
public:
	Chunck(int locx, int locy, int locz, float _blockSize, std::string seed, chunck_buffer* _buffer, int buffersId, void (*_callBack)(int i), vlibs::homogeneous_thread_pool<Chunck*>* _thread_pool);
	~Chunck();

	void init();

	Chunck* thread_work();

	BlockType getBlock(int x, int y, int z);

	inline float get_continental(float x);
	inline float get_erozion(float x);
	inline float get_peaks(float x);

	inline float parabolic_interpolation(float x, float x1, float y1, float x2, float y2);
	inline float liniar_interpolation(float x, float x1, float y1, float x2, float y2);
	inline float exponential_interpolation(float x, float x1, float y1, float x2, float y2);

	void Draw_opaque(Shader *_shader, Camera *camera, bool newPerspective);
	void Draw_transparent(Shader* _shader, Camera* camera, bool newPerspective);

	int block(int x, int y, int z);
	void setBlock(int x, int y, int z, BlockType type);

	bool isLoaded();

	bool isFailed();

	GLuint encode(GLuint x, GLuint y, GLuint z, GLuint orientation, GLuint blockType);

	void uptadateRelativePos(glm::ivec3 generatorPos);
	glm::ivec3 getRelativePos();
	glm::vec3 getRealRelativePos(glm::vec3 real_generatorPos);

private:
	std::string chunckSeed;

	int chunckX, chunckY, chunckZ;
	int relativeX, relativeY, relativeZ;
	float blockSize;
	uint8_t *blocks;

	std::vector<GLuint> faces_opaque, faces_transparent;
	int lstId;

	bool thread_done;
	bool loaded;
	bool initialized;
	bool failed;

	chunck_buffer* buffer;
	int bufferID;
	vlibs::homogeneous_thread_pool<Chunck*>* thread_pool;

	NOISE noise;

	bool isOnBorder(int x, int y, int z);

	void constructBlockFaces(int x, int y, int z);

	void addFace(Faces face, GLuint x, GLuint y, GLuint z);

	void (*callBack)(int i);
};

#endif
