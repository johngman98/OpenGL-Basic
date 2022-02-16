 #include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures,
		   unsigned int numInstances, const std::vector<glm::mat4>& instanceMatrices)
	:m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
	,m_NumInstances(numInstances)
{
	/*Setting up VAO, VBO and EBO*/
	m_VAO.bind(); //set current vertex array object
	VBO vbo(m_Vertices);//vertex buffer object
	VBO instanceVBO(instanceMatrices);//for instancing
	EBO ebo(m_Indices);//element buffer object


	//vertex attributes
	m_VAO.linkVBO(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	m_VAO.linkVBO(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
	m_VAO.linkVBO(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));
	//only link if draw more than one instance
	if(m_NumInstances > 1)
	{
		instanceVBO.bind();

		//openGL doesnt support mat4 so do 4 vec4
		m_VAO.linkVBO(instanceVBO, 3, 4, GL_FLOAT, sizeof(glm::mat4), (void*)0);
		m_VAO.linkVBO(instanceVBO, 4, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
		m_VAO.linkVBO(instanceVBO, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		m_VAO.linkVBO(instanceVBO, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		//tell opengl when to update content of vertex attribute to next element
		//1: every instance
		//0(default): based on the vertex index
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
	}

	//unbind
	//if unbind EBO before unbinding VAO, OpenGL will think we dont want the EBO
	m_VAO.unbind();
	ebo.unbind();
	instanceVBO.unbind();
	vbo.unbind();
}

Mesh::~Mesh()
{
}

void Mesh::draw(const Shader& shader, 
				const Camera& camera, 
				glm::mat4 modelMatrix, 
				glm::vec3 translation, 
				glm::quat rotation, 
				glm::vec3 scale) 
{
	/*Bind program*/
	shader.bindProgram();

	/*Bind Vertext Array Object*/
	m_VAO.bind();

	/*Bind textures*/
	//keep track of number of each type of textures
	unsigned int numDiffuses = 0;
	unsigned int numSpeculars = 0;
	for (size_t i = 0; i < m_Textures.size(); i++)
	{
		std::string num;
		std::string type = m_Textures[i].getType();
		if(type == "diffuse")
		{
			num = std::to_string(numDiffuses++);
		}
		else if(type == "specular")
		{
			num = std::to_string(numSpeculars++);
		}
		m_Textures[i].setUnit(shader, type + num, i);
		m_Textures[i].bind();
	}

	/*Take care of camera matrices*/	
	//camera position for light calculation
	shader.setUniform3f("cameraPosition", camera.getPosition());
	//projection matrix
	shader.setUniformMatrix4fv("proj", camera.calculateProjectionMatrix(45.0f, float(camera.getWindowWidth()) / float(camera.getWindowHeight()), 0.1f, 1000.0f));
	//view matrix
	shader.setUniformMatrix4fv("view", camera.calculateViewMatrix());

	if (m_NumInstances == 1)
	{
		/*Model matrix and transformation matrices*/
		//init with indentity matrices
		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rot = glm::mat4(1.0f);
		glm::mat4 sca = glm::mat4(1.0f);
		// Transform the matrices to their correct form
		trans = glm::translate(trans, translation);
		rot = glm::mat4_cast(rotation);
		sca = glm::scale(sca, scale);
		//send to shader
		shader.setUniformMatrix4fv("model", modelMatrix);
		shader.setUniformMatrix4fv("translation", trans);
		shader.setUniformMatrix4fv("rotation", rot);
		shader.setUniformMatrix4fv("scale", sca);

		/*Draw*/
		glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElementsInstanced(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0, m_NumInstances);
	}
}

