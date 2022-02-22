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
const unsigned int NUM_SAMPLES = 8;

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
	Shader framebufferShader("FramebufferVert.Shader", "FramebufferFrag.Shader");
	Shader shadowMapShader("shadowMapVert.Shader", "shadowMapFrag.Shader");
	Shader shadowCubeMapShader("shadowCubeMapVert.Shader", "shadowCubeMapFrag.Shader", "shadowCubeMapGeo.Shader");

	//model
	Model crow("models/crow/scene.gltf");

	Texture tex1("planks.png", "diffuse");
	Texture tex2("planksSpec.png", "specular");
	std::vector<Texture> textures = { tex1, tex2 };
	Mesh mesh1(vertices, indices, textures);

	//Camera
	Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), 50.0f, 0.1f, WIDTH, HEIGHT);


	//Light source attrib
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition = glm::vec3(0.0f, 20.0f, 0.0f);
	glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	//send uniforms relate to light calculation
	shader.bindProgram();
	shader.setUniform4f("lightColor", lightColor);
	shader.setUniform3f("lightPosition", lightPosition);
	shader.setUniform3f("cameraPosition", camera.getPosition());
	shader.unbindProgram();




	//for delta time
	float rotation = 0.5f;
	double deltaTime = 0.0f;
	double prevTime1 = glfwGetTime();
	double prevTime2 = glfwGetTime();
	int numFrames = 0;
	
	// Framebuffer for multi sample, cant use to post-process
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

	// Create Framebuffer Texture
	unsigned int framebufferTexture;
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, NUM_SAMPLES, GL_RGB, WIDTH, HEIGHT, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture, 0);

	// Create Render Buffer Object
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, NUM_SAMPLES, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);


	// Error checking framebuffer
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	//set unit to texture
	framebufferShader.bindProgram();
	framebufferShader.setUniform1i("screenTexture", 0);
	framebufferShader.unbindProgram();

	// Normal Framebuffer for post-processing
	//create Frame Buffer Object
	unsigned int postProcessingFBO;
	glGenFramebuffers(1, &postProcessingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

	//create Framebuffer Texture
	unsigned int postProcessingTexture;
	glGenTextures(1, &postProcessingTexture);
	glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

	//error checking framebuffer
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;


	// Framebuffer for Shadow Map
	// Shadow Map/depth Map is the texture as rendered from the light's perspective
	// Then we will use this for shadow testing (determine if a frag is in light or not)
	// Framebuffer for Shadow Map
	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	// Texture for Shadow Map FBO
	unsigned int shadowMapWidth = 2048, shadowMapHeight = 2048;
	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Prevents darkness outside the frustrum
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Matrices needed for the light's perspective
	float farPlane = 1000.0f;
	glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, farPlane);
	glm::mat4 perspectiveProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);
	glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightProjection = perspectiveProjection * lightView;

	// Since the light source doenst move, only need to be done once
	shadowMapShader.bindProgram();
	shadowMapShader.setUniformMatrix4fv("lightProjection", lightProjection);
	shadowMapShader.unbindProgram();


	// Framebuffer for Cubemap Shadow Map
	unsigned int pointShadowMapFBO;
	glGenFramebuffers(1, &pointShadowMapFBO);

	// Texture for Cubemap Shadow Map FBO
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Matrices needed for the light's perspective on all faces of the cubemap
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);
	glm::mat4 shadowTransforms[] =
	{
		shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};
	// Export all matrices to shader
	shadowCubeMapShader.bindProgram();
	shadowCubeMapShader.setUniformMatrix4fv("shadowMatrices[0]", shadowTransforms[0]);
	shadowCubeMapShader.setUniformMatrix4fv("shadowMatrices[1]", shadowTransforms[1]);
	shadowCubeMapShader.setUniformMatrix4fv("shadowMatrices[2]", shadowTransforms[2]);
	shadowCubeMapShader.setUniformMatrix4fv("shadowMatrices[3]", shadowTransforms[3]);
	shadowCubeMapShader.setUniformMatrix4fv("shadowMatrices[4]", shadowTransforms[4]);
	shadowCubeMapShader.setUniformMatrix4fv("shadowMatrices[5]", shadowTransforms[5]);
	shadowCubeMapShader.setUniform3f("lightPosition", lightPosition);
	shadowCubeMapShader.setUniform1f("farPlane", farPlane);
	shadowCubeMapShader.unbindProgram();




	//Main loop
	while (!glfwWindowShouldClose(window))
	{
		//Simple timer
		double currentTime = glfwGetTime();
		numFrames++;
		//update every second
		if (currentTime - prevTime1 >= 1.0)
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
		// Depth testing needed for Shadow Map
		// Depth testing needed for Shadow Map
		glEnable(GL_DEPTH_TEST);

		// Preparations for the Shadow Map
		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Draw scene for shadow map
		mesh1.draw(shadowCubeMapShader, camera, glm::mat4(1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec3(50.0f, 50.0f, 50.0f));
		mesh1.draw(shadowCubeMapShader, camera, glm::mat4(1.0f),
			glm::vec3(10.0f, 0.0f, 0.0f),
			glm::quat(1.0f, 1.0f, 0.0f, 0.0f),
			glm::vec3(50.0f, 50.0f, 50.0f));

		crow.draw(shadowCubeMapShader, camera);


		// Switch back to the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Switch back to the default viewport
		glViewport(0, 0, WIDTH, HEIGHT);
		// Bind the custom framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		// Specify the color of the background
		glClearColor(0.07f, 0.13f ,0.17f , 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Enable depth testing since it's disabled when drawing the framebuffer rectangle
		glEnable(GL_DEPTH_TEST);

		//Camera
		camera.processInputs(window, WIDTH, HEIGHT, deltaTime);

		//light proj to default shader
		shader.bindProgram();
		shader.setUniformMatrix4fv("lightProjection", lightProjection);
		shader.setUniform1f("farPlane", farPlane);

		// Add shadow map as texture, have to set manually since Texture obj doesnt support this
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		shader.setUniform1i("shadowCubeMap", 2);

		//Draw model
		mesh1.draw(shader, camera, glm::mat4(1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec3(50.0f, 50.0f, 50.0f));
		mesh1.draw(shader, camera, glm::mat4(1.0f),
			glm::vec3(10.0f, 0.0f, 0.0f),
			glm::quat(1.0f, 1.0f, 0.0f, 0.0f),
			glm::vec3(50.0f, 50.0f, 50.0f));

		crow.draw(shader, camera);

		// Make it so the multisampling FBO is read while the post-processing FBO is drawn
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
		// Conclude the multisampling and copy it to the post-processing FBO
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//Switch back to default framebuffer to draw the quad (contains everything we drew to the framebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		framebufferShader.bindProgram();
		glBindVertexArray(rectVAO);
		//disable depth test to make sure the quad is in front
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Swap front and back buffers
		glfwSwapBuffers(window);

		//Take care all events
		glfwPollEvents();
	}

	//cleaning up
	shader.deleteProgram();
	glDeleteFramebuffers(1, &FBO);
	glDeleteFramebuffers(1, &postProcessingFBO);
	glfwDestroyWindow(window);
	glfwTerminate();
	std::cout << "--------------END of main function-----------" << std::endl;
	return 0;
}
