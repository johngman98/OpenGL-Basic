#pragma once
#include<gl/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>
#include<vector>
#include"Texture.h" 
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Shader.h"
#include "Camera.h"




class Mesh
{
public:
	/*Constructors and Deconstructors*/

	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures);

	~Mesh();

	/*Draw method*/
	void draw(const Shader& shader,
			  const Camera& camera,
			  glm::mat4 modelMatrix = glm::mat4(1.0f),
			  glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
			  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

	/*Setters and Getters*/

private:
	void setUp() const;

private:
	/*Mesh attributes*/
	std::vector<Vertex> m_Vertices;
	std::vector<GLuint> m_Indices;
	std::vector<Texture> m_Textures;

	/*Also is the mesh ID*/
	VAO m_VAO;

	
};
