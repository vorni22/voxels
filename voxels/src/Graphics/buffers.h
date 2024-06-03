#ifndef BUFFERS_H
#define BUFFERS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>

class chunck_buffer {
public:
	chunck_buffer(unsigned int _opaque_capacity, unsigned int _transparent_capacity);
	~chunck_buffer();

	void draw_opaque();
	void draw_transparent();

	unsigned int get_opaque_count();
	unsigned int get_transparent_count();

	void assign(const std::vector<GLuint>& opaque_faces, const std::vector<GLuint>& transparent_faces);
private:

	void build_VAOs(unsigned int opaque_vao_cap);

	unsigned int opaque_VAO, transparent_VAO, VBO;

	unsigned int OPAQUE_CAPACITY;
	unsigned int TRANSPARENT_CAPACITY;
	unsigned int MAX_CAP;

	unsigned int opaque_count;
	unsigned int transparent_count;
};

#endif
