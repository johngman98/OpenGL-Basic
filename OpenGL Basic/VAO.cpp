#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &m_ID);
}

VAO::~VAO()
{
	//be very aware of this
	//glDeleteVertexArrays(1, &m_ID);
}

void VAO::linkVBO(const VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) const
{
	vbo.bind();
	//describe the buffer (attributes) so OpenGL can interpret the vertex data
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	vbo.unbind();
}

void VAO::bind() const
{
	glBindVertexArray(m_ID);
}

void VAO::unbind() const
{
	glBindVertexArray(0);
}

void VAO::del() const
{
	glDeleteVertexArrays(1, &m_ID);
}

GLuint VAO::getID() const
{
	return m_ID;
}
