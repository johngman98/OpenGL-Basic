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

	/*Cube*/
	std::vector<Vertex> cubeVert = loadObj("suzanne.obj");
	std::vector<GLuint>cubeIndices;
	for (size_t i = 0; i < cubeVert.size(); i++)
	{
		cubeIndices.push_back(i);
	}
	


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
	glDepthFunc(GL_LESS);

	//Stencil test
	glEnable(GL_STENCIL_TEST);

	//how the fragments are drawn or discarded base on content of stencil buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	//both depth test and stencil test pass use the ref value
	//decide how stencil buffer is updated
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

	//viewport: part where objects will be renders
	glViewport(0, 0, WIDTH, HEIGHT);

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//shader objects
	Shader shader("DefaultVertex.Shader", "DefaultFragment.Shader");
	Shader outLiningShader("OutLiningVert.Shader", "OutliningFrag.Shader");
	
	//texture objects 
	//vector can create copies and detroy them right after
	std::vector<Texture> textures;
	textures.reserve(2);
	textures.emplace_back("planks.png", "diffuse", 0);
	textures.emplace_back("planksSpec.png", "specular", 1);
	
	Model model1("models/crow/scene.gltf");
	//this model hasnt fixed texture path, not really needed
	Model model2("models/crow-outline/scene.gltf"); 

	//Mesh
	Mesh mesh1(vertices, indices, textures);

	//Camera
	Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), 5.0f, 0.1f, WIDTH, HEIGHT);
	
	//update matrix

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
	

	//for delta time
	float rotation = 0.5f;
	float deltaTime = 0.0f;
	float prevTime = glfwGetTime();

	

	/*Model*/
	//Model backpack("Survival_BackPack_2.fbx");

	//Main loop
	while(!glfwWindowShouldClose(window))
	{	//Set (the state) background colour
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		//Clear the back buffer and assign the new color to it
		//Clear the depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		//Simple timer
		float currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		//Camera
		camera.processInputs(window, WIDTH, HEIGHT, deltaTime);
		

		//Draw a mesh
		glStencilMask(0x00);
		mesh1.draw(shader, camera, glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f));

		//Draw a model
		//Stencil test always passed
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		model1.draw(shader, camera);
		
		//make it so only fragments with values diff than 1 pass the test
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		//disable writing to the stencil buffer to keep the prev silhouette as referrence
		//glStencilMask's mask before write to buffer, mask of glStencilFunc is used during testing
		glStencilMask(0x00);
		//Disable depth testing to make sure the next object is in front of the prev
		glDisable(GL_DEPTH_TEST);
		//outLiningShader.bindProgram();
		//outLiningShader.setUniform1f("outlining", 0.08f);
		model2.draw(outLiningShader, camera);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);



		//Draw light source
		//lightMesh.draw(lightShader, camera);
	
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
