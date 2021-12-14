#pragma once
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

/*Camera constants*/
//describe -z direction vector
const glm::vec3 DEFAULT_CAM_DIR = glm::vec3(0.0f, 0.0f, -1.0f);
const float DEFAULT_CAM_YAW = -90.0f;
const float DEFAULT_CAM_PITCH = 0.0f; 
//world up/ positive y-axis
const glm::vec3 DEFAULT_CAM_UP = glm::vec3(0.0f, 1.0f, 0.0f);


class Camera
{
public:
	/*Constuctors and deconstructors*/ 
	Camera(glm::vec3 position, float speed, float sensitivity, int windowWidth, int windowHeight);
	~Camera();

	/*Setters and Getters*/
	glm::vec3 getPosition() const;
	int getWindowWidth() const;
	int getWindowHeight() const;

	/*Camera matrices*/
	//call method outside of the game loop if the matrix doesnt need to be updated every frame
	glm::mat4 calculateViewMatrix() const;
	glm::mat4 calculateProjectionMatrix(float FOVDegree, float aspect, float nearPlane, float farPlane) const;

	/*Inputs*/
	void processInputs(GLFWwindow* window, int windowWidth, int windowHeight, float deltaTime);


private:
	void updateCameraVectors();

private:
	/*Camera attributes/vectors*/
	glm::vec3 m_Position;
	glm::vec3 m_Direction;
	glm::vec3 m_Up;

	/*Camera setting*/
	float m_Speed;
	float m_Sensitivity;

	/*Camera orientation or the m_Direction vector's orientation*/
	float m_Pitch; //angle of the direction vector with xz-plane
	float m_Yaw; //angle of the projection of direction vector onto xz-plane with x-axis
	//float m_Roll; not necessary since we only care which direction the camera points at
	
	/*For mouse inputs*/
	bool m_isFirstMouse = true;
	float m_lastMouseX = 0.0f;
	float m_lastMouseY = 0.0f;

	/*Window height and width*/
	int m_windowWidth;
	int m_windowHeight;
};