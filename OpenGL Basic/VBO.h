#pragma once
#include<gl/glew.h>
#include<glm/glm.hpp>
#include<vector>
#include<iostream>
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoordinate;
};
class VBO
{
public:
	VBO(const std::vector<Vertex>& vertices);
	~VBO();

	void bind() const; 
	void unbind() const;

	void del() const;

	GLuint getID() const;

private:
	GLuint m_ID;
};