#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
//glm
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include "Utility.h"

//TODO: make a list of checked uniform variables so dont have to call glGetUniformLocation
class Shader
{
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();
	
	void bindProgram() const;
	void unbindProgram() const;

	void deleteProgram() const;

	GLuint getProgramID();

	//set uniform's value
	//must be called after bind program
	void setUniform1i(const std::string& uniformName, GLint value) const;
	void setUniformMatrix4fv(const std::string& uniformName, const glm::mat4& matrix) const;
	void setUniform4f(const std::string& uniformName, glm::vec4 vector) const;
	void setUniform3f(const std::string& uniformName, glm::vec3 vector) const;
	void setUniform1f(const std::string& uniformName, float value) const;

private:
	//get compile errors of program or shaders
	void compileErrors(GLuint ID, const std::string& type) const;
	GLint getUniformLocation(const std::string& uniformName) const;


private:
	GLuint m_ProgramID;

};
