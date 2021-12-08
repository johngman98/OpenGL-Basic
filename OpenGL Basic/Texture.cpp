#include "Texture.h"
#include <stb_image.h>
#include <iostream>


Texture::Texture(const std::string& path, const std::string& type)
	:m_Unit(0), m_Type(type), m_ID(0) //default unit 0
{
	//read the image file and store it
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(path.c_str(), &m_Width, &m_Height, &m_NumColorChannels, 0);
	
	if (bytes != nullptr) 
	{
		//create opengl texture object without assigning it to a texture unit
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		//set parameters 
		//for when texture scale up or down 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//for when texture coords are > 1.0
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		//decide format of the data depend on number of channels
		GLenum format = 0;
		switch (m_NumColorChannels)
		{
		case 1:
			format = GL_RED;
			break;
	
		case 3:
			format = GL_RGB;
			break;

		case 4:
			format = GL_RGBA;
			break;

		default:
			break;
		}

		//assign the data loaded by stbi to opengl texture object that currently binds to GL_TEXTURE_2D
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, bytes);
		//generate mimap
		glGenerateMipmap(GL_TEXTURE_2D);

		//free the data load to bytes since opengl now has it
		stbi_image_free(bytes);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		std::cout << "Failed to load file: " << path << std::endl;
	}
	//std::cout << "Created " + type << std::endl;
}

Texture::~Texture()
{
	/*TODO: how to delete efficiently*/
	//glDeleteTextures(1, &m_ID);
	//std::cout << "Destroyed Texture with ID: " <<m_ID<< std::endl;
}

void Texture::setUnit(const Shader& shader, const std::string& uniformName, GLuint unit)  
{
	/*Up load unit to shader*/
	//only need to do this once
	m_Unit = unit;
	shader.bindProgram();
	shader.setUniform1i(uniformName, m_Unit);
	shader.unbindProgram();//just for sure
}

std::string Texture::getType() const
{
	return m_Type;
}

GLuint Texture::getUnit() const
{
	return m_Unit;
}

GLuint Texture::getID() const
{
	return m_ID;
}

void Texture::bind() const
{
	glActiveTexture(GL_TEXTURE0 + m_Unit);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
