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

float rectangleVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
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

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);

	//viewport: part where objects will be rendered
	glViewport(0, 0, WIDTH, HEIGHT);

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//shader objects
	Shader shader("DefaultVertex.Shader", "DefaultFragment.Shader");
	Shader lightShader("LightVert.Shader", "LightFrag.Shader");
	Shader framebufferShader("FramebufferVert.Shader", "FramebufferFrag.Shader");
	
	//texture objects 
	//vector can create copies and detroy them right after
	std::vector<Texture> textures;
	textures.reserve(2);
	textures.emplace_back("planks.png", "diffuse", 0);
	textures.emplace_back("planksSpec.png", "specular", 1);
	
	Model model1("models/crow/scene.gltf");



	//Mesh
	Mesh mesh1(vertices, indices, textures);

	//Camera
	Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), 5.0f, 0.1f, WIDTH, HEIGHT);

	

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


	/*Frame buffer
		Why use framebuffer?
		-To apply a global effect on the whole scene, we face a limitation: all the shaders work locally, 
		vertex shaders only know about the current vertex, and fragment shaders only know about the current pixel.

		-The only exception is when working with textures: 
		in this case, we can access any part of the texture using texture coordinates.

		-So the idea for post-processing is to first render the whole scene in a texture, 
		and then render this single texture to screen with the post-processing.
	*/

	//Prepare framebuffer rectangle VBOand VAO
	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	//Create framebufferobject and texture for framebuffer
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int framebufferTexture;
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//clamp so the one side texture wont bleed to the other side
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	//renderbuffer object: faster than texture object but cant be accessed (directly) in shader
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	//attach to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	//check errors
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	//set unit to texture
	framebufferShader.setUniform1i("screenTexture", 0);

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

		//Bind the frame buffer 1st, draw everything to framebuffer (actually to the renderbuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		
		//Set (the state) background colour
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		//Clear the back buffer and assign the new color to it
		//Clear the depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//Camera
		camera.processInputs(window, WIDTH, HEIGHT, deltaTime);
		
		//mesh1.draw(shader, camera);

		//Draw model
		model1.draw(shader, camera);

		//Switch back to default framebuffer to draw the quad (contains everything we drew to the framebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		framebufferShader.bindProgram();
		glBindVertexArray(rectVAO);
		//disable depth test to make sure the quad is in front
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, framebufferTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

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
