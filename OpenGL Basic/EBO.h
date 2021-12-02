#pragma once
#include <gl/glew.h>
#include <vector>
class EBO
{
public:
	EBO(const std::vector<GLuint>& indices);
	~EBO();

	void bind();
	void unbind();

	GLuint getID();
private:
	GLuint m_ID;
};