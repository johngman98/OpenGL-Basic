#pragma once
#include <gl/glew.h>
#include <vector>
class EBO
{
public:
	EBO(const std::vector<GLuint>& indices);
	~EBO();

	void bind() const;
	void unbind() const;

	void del() const;

	GLuint getID();
private:
	GLuint m_ID;
};