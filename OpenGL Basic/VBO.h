#pragma once
#include<gl/glew.h>
#include<glm/glm.hpp>
#include<vector>
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureUV;
};
class VBO
{
public:
	VBO(const std::vector<Vertex>& vertices);
	~VBO();

	void bind() const; 
	void unbind() const;

	GLuint getID() const;

private:
	GLuint m_ID;
};