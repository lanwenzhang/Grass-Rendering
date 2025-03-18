#include "shader.h""
#include "../wrapper/checkError.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath){
	
    // Save the vertex and fragment shader code 
	std::string vertexCode;
	std::string fragmentCode;

	// Read vertex and fragment shader file
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// Monitor errors
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {

		// Open the file
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		// Read the file
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();


		// Close the file
		vShaderFile.close();
		fShaderFile.close();

		// Save to the code
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}

	catch (std::ifstream::failure& e) {
		std::cout << "ERROR: Shader File ERROR: " << e.what() << std::endl;
	}

    const char* vetexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    // 1 Create shader variable
    GLuint vertex, fragment;
    vertex = GL_CALL(glCreateShader(GL_VERTEX_SHADER));
    fragment = GL_CALL(glCreateShader(GL_FRAGMENT_SHADER));


    // 2 Input shader codes to variable
    GL_CALL(glShaderSource(vertex, 1, &vetexShaderSource, NULL));
    GL_CALL(glShaderSource(fragment, 1, &fragmentShaderSource, NULL));


    // 4 Compile shader and check error
    GL_CALL(glCompileShader(vertex));
    checkShaderErrors(vertex, "COMPILE");

    GL_CALL(glCompileShader(fragment));
    checkShaderErrors(fragment, "COMPILE");

    // 5 Create a rendering program
    mProgram = glCreateProgram();

    // 6 Input compiled shaders
    GL_CALL(glAttachShader(mProgram, vertex));
    GL_CALL(glAttachShader(mProgram, fragment));

    // 7 Link program and check errors
    GL_CALL(glLinkProgram(mProgram));
    checkShaderErrors(mProgram, "LINK");

    // 8 Clear
    GL_CALL(glDeleteShader(vertex));
    GL_CALL(glDeleteShader(fragment));
}

Shader::~Shader(){
}

void Shader::begin() {
	GL_CALL(glUseProgram(mProgram));
}

void Shader::end() {
	GL_CALL(glUseProgram(0));	
}

void Shader::setFloat(const std::string& name, float value) {
    
    // 1 Get the uniform location
    GLint location = glGetUniformLocation(mProgram, name.c_str());

    // 2 Input value to uniform
    glUniform1f(location, value);
}

void Shader::setVector3(const std::string& name, float x, float y, float z) {

    GLint location = glGetUniformLocation(mProgram, name.c_str());

    glUniform3f(location, x, y, z);
}

void Shader::setVector3(const std::string& name, const float* values) {
    
    GLint location = glGetUniformLocation(mProgram, name.c_str());

    glUniform3fv(location, 1, values);
}

void Shader::setVector3(const std::string& name, const glm::vec3 value) {

    GLint location = glGetUniformLocation(mProgram, name.c_str());

    glUniform3f(location, value.x, value.y, value.z);

}

void Shader::setInt(const std::string& name, int value) {

    GLint location = glGetUniformLocation(mProgram, name.c_str());

    glUniform1i(location, value);
}

void Shader::setMatrix4x4(const std::string& name, glm::mat4 value) {

    GLint location = glGetUniformLocation(mProgram, name.c_str());

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMatrix4x4Array(const std::string& name, glm::mat4* value, int count) {

    GLint location = glGetUniformLocation(mProgram, name.c_str());

    glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(value[0]));

}

void Shader::setMatrix3x3(const std::string& name, glm::mat3 value) {

    GLint location = glGetUniformLocation(mProgram, name.c_str());

    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::checkShaderErrors(GLuint target, std::string type) {
    int success = 0;
    char infoLog[1024];

    if (type == "COMPILE") {
        glGetShaderiv(target, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(target, 1024, NULL, infoLog);
            std::cout << "Error: SHADER COMPILE ERROR" << "\n" << infoLog << std::endl;
        }
    }
    else if (type == "LINK") {
        glGetProgramiv(target, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(target, 1024, NULL, infoLog);
            std::cout << "Error: LINK ERROR" << "\n" << infoLog << std::endl;
        }
    }
    else {
        std::cout << "Error: Check shader errors type is wrong" << std::endl;
    }

}