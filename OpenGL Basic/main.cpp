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
#include <stb_image.h>


const int WIDTH = 800;
const int HEIGHT = 800;

std::vector<Vertex> vertices =
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
	//only use this line if a framebuffer is not used
	//glfwWindowHint(GLFW_SAMPLES, NUM_SAMPLES);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//ONLY modern functions
	


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
	// Enable Multi Sampling
	//glEnable(GL_MULTISAMPLE);
	// Enable Cull Facing
	//glEnable(GL_CULL_FACE);
	// Keeps front faces
	//glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	//glFrontFace(GL_CCW);

	//viewport: part where objects will be rendered
	glViewport(0, 0, WIDTH, HEIGHT);

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//shader objects
	Shader shader("DefaultVertex.Shader", "DefaultFragment.Shader");
	

	//model
	//Model crow("models/crow/scene.gltf");

	Texture tex1("planks.png", "diffuse");
	Texture tex2("planksSpec.png", "specular");
	std::vector<Texture> textures = { tex1, tex2 };
	Mesh mesh1(vertices, indices, textures);

	//Camera
	Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), 5.0f, 0.1f, WIDTH, HEIGHT);


	//Light source attrib
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition = glm::vec3(0.0f, 0.5f, 0.0f);
	glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	//send uniforms relate to light calculation
	shader.bindProgram();
	shader.setUniform4f("lightColor", lightColor);
	shader.setUniform3f("lightPosition", lightPosition);
	shader.setUniform3f("cameraPosition", camera.getPosition());
	shader.unbindProgram();

	//light
	Shader lightShader("LightVert.Shader", "LightFrag.Shader");
	//light mesh
	Mesh lightMesh(lightVertices, lightIndices, textures);
	lightShader.bindProgram();
	lightShader.setUniform4f("lightColor", lightColor);
	lightShader.unbindProgram();


	//for delta time
	float rotation = 0.5f;
	double deltaTime = 0.0f;
	double prevTime1 = glfwGetTime();
	double prevTime2 = glfwGetTime();
	int numFrames = 0;
	

	

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Camera
		camera.processInputs(window, WIDTH, HEIGHT, deltaTime);
		
		//mesh1.draw(shader, camera);

		//Draw model
		mesh1.draw(shader, camera, glm::mat4(1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 2.0f));
		lightMesh.draw(lightShader, camera,  glm::mat4(1.0f),
			lightPosition,
			glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f));
	

		//Swap front and back buffers
		glfwSwapBuffers(window);

		//Take care all events
		glfwPollEvents();
	}

	//cleaning up
	shader.deleteProgram();

	glfwDestroyWindow(window);
	glfwTerminate();
	std::cout << "--------------END of main function-----------" << std::endl;
	return 0;
}
