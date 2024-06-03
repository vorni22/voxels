#include "buffers.h"

chunck_buffer::chunck_buffer(unsigned int _opaque_capacity, unsigned int _transparent_capacity){
	OPAQUE_CAPACITY = _opaque_capacity;
	TRANSPARENT_CAPACITY = _transparent_capacity;
	MAX_CAP = OPAQUE_CAPACITY + TRANSPARENT_CAPACITY;
	
	opaque_count = 0;
	transparent_count = 0;

	// tell opengl to generate VAOs
	glGenVertexArrays(1, &opaque_VAO);
	glGenVertexArrays(1, &transparent_VAO);

	// generate a VBO
	glGenBuffers(1, &VBO);

	// bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// allocate memory for the VBO
	// The memory will never exide opaque cap + transparent cap
	glBufferData(GL_ARRAY_BUFFER, (MAX_CAP) * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

	// Set atribute pointers for the 2 VAOs

	// bind opaque VAO
	glBindVertexArray(opaque_VAO);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLuint), (void*)0);
	glEnableVertexAttribArray(0);

	// bind transparent VAO
	glBindVertexArray(transparent_VAO);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLuint), (void*)(OPAQUE_CAPACITY * sizeof(GLuint)));
	glEnableVertexAttribArray(0);
}

chunck_buffer::~chunck_buffer(){
	glDeleteVertexArrays(1, &opaque_VAO);
	glDeleteVertexArrays(1, &transparent_VAO);
	glDeleteBuffers(1, &VBO);
}

void chunck_buffer::draw_opaque(){
	glBindVertexArray(opaque_VAO);
	glDrawArrays(GL_POINTS, 0, opaque_count);
}

void chunck_buffer::draw_transparent(){
	glBindVertexArray(transparent_VAO);
	glDrawArrays(GL_POINTS, 0, transparent_count);
}

unsigned int chunck_buffer::get_opaque_count(){
	return opaque_count;
}

unsigned int chunck_buffer::get_transparent_count(){
	return transparent_count;
}

void chunck_buffer::assign(const std::vector<GLuint>& opaque_faces, const std::vector<GLuint>& transparent_faces){
	if ((unsigned int)opaque_faces.size() + (unsigned int)transparent_faces.size() > MAX_CAP) {
		std::cout << "sk";
		return;
	}

	opaque_count = (unsigned int)opaque_faces.size();
	transparent_count = (unsigned int)transparent_faces.size();

	bool sw = false;

	if (opaque_count > OPAQUE_CAPACITY) {
		OPAQUE_CAPACITY = opaque_count;
		TRANSPARENT_CAPACITY = MAX_CAP - OPAQUE_CAPACITY;
		sw = true;
	}

	if (transparent_count > TRANSPARENT_CAPACITY) {
		TRANSPARENT_CAPACITY = transparent_count;
		OPAQUE_CAPACITY = MAX_CAP - TRANSPARENT_CAPACITY;
		sw = true;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	if (opaque_count != 0)
		glBufferSubData(GL_ARRAY_BUFFER, 0, opaque_count * sizeof(GLuint), &opaque_faces[0]);

	if (transparent_count != 0)
		glBufferSubData(GL_ARRAY_BUFFER, OPAQUE_CAPACITY * sizeof(GLuint), transparent_count * sizeof(GLuint), &transparent_faces[0]);

	if (sw) {
		build_VAOs(OPAQUE_CAPACITY);
	}
}

void chunck_buffer::build_VAOs(unsigned int opaque_vao_cap){
	// bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// no need to ever change the first VAO

	// bind transparent VAO
	glBindVertexArray(transparent_VAO);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLuint), (void*)(opaque_vao_cap * sizeof(GLuint)));
	glEnableVertexAttribArray(0);
}



