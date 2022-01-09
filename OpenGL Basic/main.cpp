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
#include<algorithm>
#include <map>

//user-defined headers
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Utility"


const int WIDTH = 800;
const int HEIGHT = 800;



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

	//Depth test
	//depth value is 0.0f at near plane and 1.0f at far plane
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//viewport: part where objects will be renders
	glViewport(0, 0, WIDTH, HEIGHT);

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//shader objects
	Shader shader("DefaultVertex.Shader", "DefaultFragment.Shader");
	Shader outLiningShader("OutLiningVert.Shader", "OutliningFrag.Shader");
	Shader grassShader("DefaultVertex.Shader", "GrassFrag.Shader");
	Shader windowsShader("DefaultVertex.Shader", "WindowsFrag.Shader");
	
	//texture objects 
	//vector can create copies and detroy them right after
	std::vector<Texture> textures;
	textures.reserve(2);
	textures.emplace_back("planks.png", "diffuse", 0);
	textures.emplace_back("planksSpec.png", "specular", 1);
	
	Model ground("models/ground/scene.gltf");
	Model windows("models/windows/scene.gltf");
	Model grass("models/grass/scene.gltf");


	//Mesh
	Mesh mesh1(vertices, indices, textures);

	//Camera
	Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), 5.0f, 0.1f, WIDTH, HEIGHT);
	
	//Shaders
	Shader lightShader("LightVert.Shader", "LightFrag.Shader");

	//light mesh
	Mesh lightMesh(lightVertices, lightIndices);



	//Light source object
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	//send uniforms relate to light calculation
	shader.bindProgram();
	shader.setUniform4f("lightColor", lightColor);
	shader.setUniform3f("lightPosition", lightPosition);
	shader.setUniform3f("cameraPosition", camera.getPosition());
	shader.unbindProgram();

	lightShader.bindProgram();
	lightShader.setUniform4f("lightColor", lightColor);
	lightShader.unbindProgram();

	grassShader.bindProgram();
	grassShader.setUniform4f("lightColor", lightColor);
	grassShader.setUniform3f("lightPosition", lightPosition);
	grassShader.setUniform3f("cameraPosition", camera.getPosition());
	grassShader.unbindProgram();

	

	//for delta time
	float rotation = 0.5f;
	double deltaTime = 0.0f;
	double prevTime1 = glfwGetTime();
	double prevTime2 = glfwGetTime();
	int numFrames = 0;
	

	/*Windows' positions*/
	std::vector<glm::vec3> windowsPos
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	//Main loop
	while(!glfwWindowShouldClose(window))
	{	
		//Simple timer
		double currentTime = glfwGetTime();
		numFrames++;
		//update every second
		if(currentTime - prevTime1 >= 1.0)
		{
			std::string FPS = std::to_string(numFrames);
			std::string ms = std::to_string(1000.0 / numFrames);
			std::string newTitle = "Performance: " + FPS + " FPS/ " + ms + " ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			//reset
			numFrames = 0;
			//update prev
			prevTime1 += 1.0;//or 1.0
		}
		deltaTime = currentTime - prevTime2;
		prevTime2 = currentTime;
		//Set (the state) background colour
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		//Clear the back buffer and assign the new color to it
		//Clear the depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		

		//Camera
		camera.processInputs(window, WIDTH, HEIGHT, deltaTime);
		
		//mesh1.draw(shader, camera);

		//Draw model
		ground.draw(shader, camera);
		grass.draw(grassShader, camera);
		
		//sort according to distance from cam to the windows
		/*std::sort(windowsPos.begin(), windowsPos.end(), [&](glm::vec3 v1, glm::vec3 v2)
			{
				float d1 = glm::length(camera.getPosition() - v1);
				float d2 = glm::length(camera.getPosition() - v2);
				return d1 > d2;
			});
		glEnable(GL_BLEND);
		for(auto& pos: windowsPos)
		{
			//render the furthest first
			windows.draw(windowsShader, camera, pos);
		}
		glDisable(GL_BLEND);

		
		*/
		//using map 
		std::map<float, glm::vec3> posSorted;
		for (auto& pos : windowsPos)
		{
			float distance = glm::length(camera.getPosition() - pos);
			posSorted[distance] = pos;
		}


		glEnable(GL_BLEND);
		for(auto it = posSorted.rbegin(); it != posSorted.rend(); it++)
		{
			//render the furthest first
			windows.draw(windowsShader, camera, it->second);
		}
		glDisable(GL_BLEND);

		//Swap front and back buffers
		glfwSwapBuffers(window);

		//Take care all events
		glfwPollEvents();
	}

	//cleaning up
	lightShader.deleteProgram();
	shader.deleteProgram();

	glfwDestroyWindow(window);
	glfwTerminate();
	std::cout << "--------------END of main function-----------" << std::endl;
	return 0;
}
