//glew
#define GLEW_STATIC
#include <gl/glew.h>

//glfw
#include <GLFW/glfw3.h>

//glm
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

//sti headers
#include <iostream>
#include<sstream>

//user-defined headers
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"
#include "Mesh.h"


const int WIDTH = 800;
const int HEIGHT = 800;

std::vector<Vertex> loadObj(const std::string& filePath)
{
	std::fstream fileStream(filePath);
	/*check state of the file stream*/
	if (!fileStream.good())
		std::cout << "File not found!!" << std::endl;
	
	/*extract file content*/
	//temporary line
	std::string line;
	//to store actual data
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoordinates;
	//to store which indices to used
	std::vector<GLuint> posIndices;
	std::vector<GLuint> normIndices;
	std::vector<GLuint> texCoordIndices;
	while (!fileStream.eof())
	{
		//read file stream and store in line
		std::getline(fileStream, line);

		//get first two chars
		std::string prefix = line.substr(0, 2);
		//position data
		if(prefix == "v ")
		{
			std::stringstream stream(line.substr(2));
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			stream >> x >> y >> z;
			positions.push_back(glm::vec3(x, y, z));
		}
		//normal data
		else if (prefix == "vn")
		{
			std::stringstream stream(line.substr(2));
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			stream >> x >> y >> z;
			normals.push_back(glm::vec3(x, y, z));
		}
		//texture coordinate data
		else if (prefix == "vt")
		{
			std::stringstream stream(line.substr(2));
			float x = 0.0f;
			float y = 0.0f;
			stream >> x >> y;
			textureCoordinates.push_back(glm::vec2(x, y));
		}
		//face => indices
		else if (prefix == "f ")
		{
			std::stringstream stream(line.substr(2));
			std::string vert;
			//each line of face contain 3 vertices: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
			while(stream>>vert)
			{
				size_t pos1 = vert.find_first_of('/');
				size_t pos2 = vert.find_first_of('/', pos1 + 1);


				posIndices.push_back(std::stoul(vert.substr(0, pos1)));
				texCoordIndices.push_back(std::stoul(vert.substr(pos1 + 1, pos2 - pos1 - 1)));
				normIndices.push_back(std::stoul(vert.substr(pos2 + 1)));
				
				/*TODO: handle where missing postion, normal or uv coord*/
			}
		}

	}

	/*Populate the vertices vector w.r.t indices*/
	std::vector<Vertex> result;
	//size of posIndices, texCoordIndices and normIndices should be equal
	result.reserve(posIndices.size());
	for (size_t i = 0; i < posIndices.size(); i++)
	{
		//obj format: index starts at 1
		glm::vec3 pos = positions[posIndices[i] - 1];
		glm::vec3 norm = normals[normIndices[i] - 1];
		glm::vec2 texCoord = textureCoordinates[texCoordIndices[i] - 1];
		result.push_back({ pos, norm, texCoord });

	}

	/*for(auto v: result)
	{
		std::cout << "pos: " << v.position.x << v.position.y << v.position.z << std::endl;
		std::cout << "norm: " << v.normal.x << v.normal.y << v.normal.z << std::endl;
		std::cout << "tex: " << v.textureUV.x << v.textureUV.y << std::endl;
	}*/
	return result;
}

int main(void)
{
	//init
	if(!glfwInit())
	{
		std::cout << "FAILED to initialise GLFW" << std::endl;
		return -1;
	}

	//window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//ONLY modern functions
	
	std::vector<Vertex> vertices  =     
	{ //               COORDINATES           /           Normal         /       uvs         //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f, -1.0f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f,  1.0f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)}
	};
	

	std::vector<GLuint> indices =
	{
		0, 1, 2,
		0, 2, 3
	};

	//light source
	std::vector<Vertex> lightVertices =
	{ //     COORDINATES     //
		Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
		Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
		Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
		Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
	};
	std::vector<GLuint> lightIndices =
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		2, 6, 5,
		2, 5, 1,
		1, 5, 4,
		1, 4, 0,
		4, 5, 6,
		4, 6, 7
	};




	//window object
	//The GLFWwindow object encapsulates both a window and a context
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "unclover", NULL, NULL);
	if(window == NULL)
	{
		std::cout << "FAILED to create GLFWwindow object" << std::endl;
		glfwTerminate();
		return -1;
	}
	//context holds all states and obects (texture, shaders,.etc)
	//the current context is the context of the assigned window object
	glfwMakeContextCurrent(window);
	
	//initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		std::cout << "FAILED to initialize GlEW" << std::endl;
		return -1;
	}

	//opengl settings
	glEnable(GL_DEPTH_TEST);

	//viewport: part where objects will be renders
	glViewport(0, 0, WIDTH, HEIGHT);

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//shader objects
	Shader shader("DefaultVertex.Shader", "DefaultFragment.Shader");
	
	//texture objects 
	//vector can create copies and detroy them right after
	std::vector<Texture> textures;
	textures.reserve(2);
	textures.emplace_back("planks.png", "diffuse");
	textures.emplace_back("planksSpec.png", "specular");
	//assign texture units do once per shader
	

	
	//Mesh
	Mesh mesh1(vertices, indices, textures);
	mesh1.setUp(shader);

	//Camera
	Camera camera(glm::vec3(0.0f, 1.0f, 2.0f), 5.0f, 0.1f);
	
	//update matrix
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.calculateViewMatrix();
	glm::mat4 proj = camera.calculateProjectionMatrix(45.0f, float(WIDTH) / float(HEIGHT), 0.1f, 100.0f);
	

	shader.bindProgram();
	shader.setUniformMatrix4fv("model", model);
	shader.setUniformMatrix4fv("view", view);
	shader.setUniformMatrix4fv("proj", proj);
	shader.unbindProgram();


	//Light source object
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);

	Shader lightShader("LightVert.Shader", "LightFrag.Shader");
	
	//light mesh
	Mesh lightMesh(lightVertices, lightIndices);
	lightMesh.setUp(lightShader);

	glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightPosition);
	glm::mat4 lightView = view; 
	glm::mat4 lightProj = proj;

	lightShader.bindProgram();
	lightShader.setUniformMatrix4fv("lightModel", lightModel);
	lightShader.setUniformMatrix4fv("lightView", lightView);
	lightShader.setUniformMatrix4fv("lightProj", lightProj);
	lightShader.unbindProgram();

	//send uniforms
	shader.bindProgram();
	shader.setUniform4f("lightColor", lightColor);
	shader.setUniform3f("lightPosition", lightPosition);
	shader.setUniform3f("cameraPosition", camera.getPosition());
	shader.unbindProgram();

	lightShader.bindProgram();
	lightShader.setUniform4f("lightColor", lightColor);
	lightShader.unbindProgram();
	

	//for delta time
	float rotation = 0.5f;
	float deltaTime = 0.0f;
	float prevTime = glfwGetTime();

	

	/*Model*/
	//Model backpack("Survival_BackPack_2.fbx");

	//Main loop
	while(!glfwWindowShouldClose(window))
	{	//Set (the state) background colour
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		//Clear the back buffer and assign the new color to it
		//deph buffer has the value of the biggest z value (Opengl z-axis goes toward screen) of the last scene 
		//need to check again if there is a vertex has smaller z value than the previous one
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Simple timer
		float currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		//Camera
		camera.processInputs(window, WIDTH, HEIGHT, deltaTime);
		glm::mat4 camView = camera.calculateViewMatrix();
		
		//Draw an object
		shader.bindProgram();
		//update cam pos for lighting
		shader.setUniform3f("cameraPosition", camera.getPosition());
		shader.setUniformMatrix4fv("view", camView);
		mesh1.Draw(shader);
	
		//Draw light source
		lightShader.bindProgram();
		lightShader.setUniformMatrix4fv("lightView", camView);
		lightMesh.Draw(lightShader);
	
		
		
		//using draw array
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		
		//Swap front and back buffers
		glfwSwapBuffers(window);

		//Take care all events
		glfwPollEvents();
	}

	//cleaning up
	glfwDestroyWindow(window);
	glfwTerminate();
	std::cout << "--------------END of main function-----------" << std::endl;
	return 0;
}
