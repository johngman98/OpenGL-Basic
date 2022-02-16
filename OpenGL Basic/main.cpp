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

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
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
	Shader asteroidShader("AsteroidVert.Shader", "DefaultFragment.Shader");
	Shader skyboxShader("SkyboxVert.Shader", "SkyboxFrag.Shader");

	//model
	Model jupiter("models/jupiter/scene.gltf");
	

	//Camera
	Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), 5.0f, 0.1f, WIDTH, HEIGHT);


	//Light source attrib
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	//send uniforms relate to light calculation
	shader.bindProgram();
	shader.setUniform4f("lightColor", lightColor);
	shader.setUniform3f("lightPosition", lightPosition);
	shader.setUniform3f("cameraPosition", camera.getPosition());
	shader.unbindProgram();

	asteroidShader.bindProgram();
	asteroidShader.setUniform4f("lightColor", lightColor);
	asteroidShader.setUniform3f("lightPosition", lightPosition);
	asteroidShader.setUniform3f("cameraPosition", camera.getPosition());
	asteroidShader.unbindProgram();

	//skybox texture unit
	skyboxShader.bindProgram();
	skyboxShader.setUniform1i("skybox", 0);
	

	//for delta time
	float rotation = 0.5f;
	double deltaTime = 0.0f;
	double prevTime1 = glfwGetTime();
	double prevTime2 = glfwGetTime();
	int numFrames = 0;
	
	//sky box
	//Create VAO, VBO and EBO
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//faces' of cubemap
	std::string facesCubemap[6] =
	{
		"skybox textures/space/right.png",
		"skybox textures/space/left.png",
		"skybox textures/space/top.png",
		"skybox textures/space/bottom.png",
		"skybox textures/space/front.png",
		"skybox textures/space/back.png",
	};

	// Cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//Iterate thru all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			//cube map texture start at top left
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				//cubemap's front is positive z
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	

	// The number of asteroids to be created
	const unsigned int numInstances = 5000;
	// Radius of circle around which asteroids orbit
	float radius = 100.0f;
	// How much ateroids deviate from the radius
	float radiusDeviation = 25.0f;

	std::vector<glm::mat4> instanceMatrices;
	

	for (unsigned int i = 0; i < numInstances; i++)
	{
		// Generates x and y for the function x^2 + y^2 = radius^2 which is a circle
		float x = randf();
		float finalRadius = radius + randf() * radiusDeviation;
		float y = ((rand() % 2) * 2 - 1) * sqrt(1.0f - x * x);

		glm::vec3 tempTranslation;
		glm::quat tempRotation;
		glm::vec3 tempScale;

		// Makes the random distribution more even
		if (randf() > 0.5f)
		{
			// Generates a translation near a circle of radius "radius"
			tempTranslation = glm::vec3(y * finalRadius, randf(), x * finalRadius);
		}
		else
		{
			// Generates a translation near a circle of radius "radius"
			tempTranslation = glm::vec3(x * finalRadius, randf(), y * finalRadius);
		}

		// Generates random rotations
		tempRotation = glm::quat(1.0f, randf(), randf(), randf());
		// Generates random scales
		tempScale = 0.1f * glm::vec3(randf(), randf(), randf());

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rot = glm::mat4(1.0f);
		glm::mat4 sca = glm::mat4(1.0f);
		// Transform the matrices to their correct form
		trans = glm::translate(trans, tempTranslation);
		rot = glm::mat4_cast(tempRotation);
		sca = glm::scale(sca, tempScale);

		instanceMatrices.push_back(trans * rot *sca);
	}
	Model asteroid("models/asteroid/scene.gltf", numInstances, instanceMatrices);

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
		glEnable(GL_DEPTH_TEST);

		//Camera
		camera.processInputs(window, WIDTH, HEIGHT, deltaTime);
		
		//mesh1.draw(shader, camera);

		//Draw model
		jupiter.draw(shader, camera);
		asteroid.draw(asteroidShader, camera);
		
		//Cube map
		//since the cubemap will always have the depth of 1.0, we need equal sign so it doesnt get discarded
		glDepthFunc(GL_LEQUAL);

		skyboxShader.bindProgram();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
		//we make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
		//the last row and column affect the translation of the skybox (which we don't want to affect)
		view = glm::mat4(glm::mat3(camera.calculateViewMatrix()));
		proj = camera.calculateProjectionMatrix(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);
		skyboxShader.setUniformMatrix4fv("view", view);
		skyboxShader.setUniformMatrix4fv("proj", proj);

		//draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		//where an object is present (a depth of 1.0f will always fail against any object's depth value)
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//switch back
		glDepthFunc(GL_LESS);

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
