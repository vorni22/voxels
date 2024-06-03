#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	unsigned int id;

	Shader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShader = "");
	void activate();

	//utility
	std::string loadShaderSrc(const char* filepath);
	GLuint compileShader(const char* filepath, GLenum type);

	//uniform functions
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void set3Float(const std::string& name, float v1, float v2, float v3);
    void set3Float(const std::string& name, glm::vec3 v);
    void set4Float(const std::string& name, float v1, float v2, float v3, float v4);
    //void set4Float(const std::string& name, aiColor4D color);
    void set4Float(const std::string& name, glm::vec4 v);
    void setMat3(const std::string& name, glm::mat3 val);
    void setMat4(const std::string& name, glm::mat4 val);
};

#endif
