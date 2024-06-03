#include "shader.h"

// CONSTRUCTOR

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath){
	int succes;
	char infoLog[512];

	GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);

	if (geometryShaderPath != "") {
		GLuint geoShader = compileShader(geometryShaderPath, GL_GEOMETRY_SHADER);
		glAttachShader(id, geoShader);
	}

	glAttachShader(id, fragShader);

	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &succes);
	if (!succes) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "Error while creating the shader program (linking): " << std::endl << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

//UTILITY

void Shader::activate(){
	glUseProgram(id);
}

std::string Shader::loadShaderSrc(const char* filepath){
	std::ifstream file;
	std::stringstream buf;

	std::string ret = "";

	file.open(filepath);

	if (file.is_open()) {
		buf << file.rdbuf();
		ret = buf.str();
	}
	else {
		std::cout << "Could not open the file '" << filepath << "';" << std::endl;
	}

	file.close();

	return ret;
}

GLuint Shader::compileShader(const char* filepath, GLenum type){
	int succes;
	char infoLog[512];
	
	GLuint shader = glCreateShader(type);
	std::string vertShaderSrc = loadShaderSrc(filepath);
	const GLchar* ShaderCode = vertShaderSrc.c_str();
	glShaderSource(shader, 1, &ShaderCode, NULL);
	glCompileShader(shader); //compile it

	//catch errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &succes);
	if (!succes) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Error while compiling the vertex shader: " << std::endl << infoLog << std::endl;
	}

	return shader;
}


// UNFORMS

void Shader::setBool(const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set3Float(const std::string& name, float v1, float v2, float v3) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set3Float(const std::string& name, glm::vec3 v) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z);
}

void Shader::set4Float(const std::string& name, float v1, float v2, float v3, float v4) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}

/*
void Shader::set4Float(const std::string& name, aiColor4D color) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), color.r, color.g, color.b, color.a);
}
*/

void Shader::set4Float(const std::string& name, glm::vec4 v) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::setMat3(const std::string& name, glm::mat3 val) {
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setMat4(const std::string& name, glm::mat4 val) {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}
