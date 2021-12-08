#include "VBO.h"

VBO::VBO(const std::vector<Vertex>& vertices)
{
	//gen buffer
	glGenBuffers(1, &m_ID);
	//bind buffer to curreunt binding point/target (GL_ARRAY_BUFFER)
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	//allocate and stream data to the buffer
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

VBO::~VBO()
{
	//be very aware of this
	//glDeleteBuffers(1, &m_ID);
	//std::cout << "Deleted VBO" << std::endl;
}

void VBO::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VBO::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::del() const
{
	glDeleteBuffers(1, &m_ID);
}

GLuint VBO::getID() const
{
	return m_ID;
}
