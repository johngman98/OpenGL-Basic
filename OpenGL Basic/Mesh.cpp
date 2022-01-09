 #include "Mesh.h"



Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
	:m_Vertices(vertices), m_Indices(indices), m_Textures({})
{
	//VBOs and EBOs are in setUp's scope which can be problematic
	setUp();
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures)
	:m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
{
	setUp();
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
	shader.setUniformMatrix4fv("proj", camera.calculateProjectionMatrix(45.0f, float(camera.getWindowWidth()) / float(camera.getWindowHeight()), 0.1f, 100.0f));
	//view matrix
	shader.setUniformMatrix4fv("view", camera.calculateViewMatrix());

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

void Mesh::setUp() const
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

}
