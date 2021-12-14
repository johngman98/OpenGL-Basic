#pragma once
#include <string>
#include <gl/glew.h>
#include "Shader.h"
class Texture
{
public:
	/*Constructors and Deconstuctors*/

	Texture(const std::string& path, const std::string& type, GLuint slot = 0);
	~Texture();

	/*(un)bind methods*/
	void bind() const;
	void unbind() const;

	/*Setters and getters*/
	void setUnit(const Shader& shader, const std::string& uniformName, GLuint unit);
	std::string getType() const;
	GLuint getUnit() const;
	GLuint getID() const;

private:
	GLuint m_ID;
	GLuint m_Unit;
	std::string m_Type;
	GLint m_Width, m_Height, m_NumColorChannels;

};