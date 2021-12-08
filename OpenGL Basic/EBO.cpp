#include "EBO.h"

EBO::EBO(const std::vector<GLuint>& indices)
{
	//gen buffer
	glGenBuffers(1, &m_ID);
	//bind buffer to curreunt binding point/target (GL_ELEMENT_ARRAY_BUFFER)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	//allocate and stream data to the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

EBO::~EBO()
{
	//be very aware of this
	//glDeleteBuffers(1, &m_ID);
}

void EBO::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void EBO::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::del() const
{
	glDeleteBuffers(1, &m_ID);
}

GLuint EBO::getID()
{
	return m_ID;
}
