#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

class VertexBuffer {
public:
	VertexBuffer();
	~VertexBuffer();

	void writeToBuffer(void* data);
	void bind();

	(void*) MapToPointer();
	(void*) UnmapPointer();

private:
	unsigned int VAO, VBO;

	bool maped;

	void init();
};

#endif 
