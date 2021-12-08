#pragma once
#include "VBO.h"
#include <GL/glew.h>

class VAO
{
public:
	VAO();
	~VAO();
	
	void linkVBO(const VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) const;
	void bind() const;
	void unbind() const;
	void del () const;

	GLuint getID() const;
private:
	GLuint m_ID;
};