#pragma once
#include<gl/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
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
	void Draw(const Shader& shader) const;

	/*Setters and Getters*/
	void setUp(const Shader& shader);

private:
	/*Mesh attributes*/
	std::vector<Vertex> m_Vertices;
	std::vector<GLuint> m_Indices;
	std::vector<Texture> m_Textures;

	/*Also is the mesh ID*/
	VAO m_VAO;

	/*To check if the mesh has been set up*/
	bool m_IsSetUp; 
};
