#include "Mesh.h"



Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const Shader& shader)
	:m_Vertices(vertices), m_Indices(indices), m_Textures({})
{
	//VBOs and EBOs are in setUp's scope which can be problematic
	setUp(shader);
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures, const Shader& shader)
	:m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
{
	setUp(shader);
}

Mesh::~Mesh()
{
}

void Mesh::Draw(const Shader& shader, const Camera& camera, bool isLightSource) const
{
	/*Bind program*/
	shader.bindProgram();

	/*Bind Vertext Array Object*/
	m_VAO.bind();

	/*Bind textures*/
	for (size_t i = 0; i < m_Textures.size(); i++)
	{
		m_Textures[i].bind();
	}

	/*Take care of camera matrices*/	
	//camera position for light calculation
	if (!isLightSource)
	{
		shader.setUniform3f("cameraPosition", camera.getPosition());
	}
	//TODO: need to add perspective and model matrix
	//view matrix
	shader.setUniformMatrix4fv("view", camera.calculateViewMatrix());

	/*Draw*/
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::setUp(const Shader& shader) 
{
	/*Setting up VAO, VBO and EBO*/
	m_VAO.bind(); //set current vertex array object
	VBO vbo(m_Vertices);//vertex buffer object
	EBO ebo(m_Indices);//element buffer object

	//vertex attributes
	m_VAO.linkVBO(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	m_VAO.linkVBO(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
	m_VAO.linkVBO(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));
	
	//unbind
	//if unbind EBO before unbinding VAO, OpenGL will think we dont want the EBO
	m_VAO.unbind();
	ebo.unbind();
	vbo.unbind();

	/*Setting up texture unit*/
	int numDiffuse = 0;
	int numSpecular = 0;
	for (size_t i = 0; i < m_Textures.size(); i++)
	{
		std::string type = m_Textures[i].getType();
		std::string num;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		else
		{
			std::cout << "There is no texture type of: " << type << std::endl;
		}
		m_Textures[i].setUnit(shader, type + num, i);
	}
	
}
