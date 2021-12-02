#include "Camera.h"


Camera::Camera(glm::vec3 position, float speed, float sensitivity)
	:m_Position(position), m_Direction(DEFAULT_CAM_DIR), m_Up(DEFAULT_CAM_UP),
	 m_Speed(speed), m_Sensitivity(sensitivity),
	 m_Pitch(DEFAULT_CAM_PITCH), m_Yaw(DEFAULT_CAM_YAW)
{
}

Camera::~Camera()
{
}

glm::vec3 Camera::getPosition() const
{
	return m_Position;
}
 
glm::mat4 Camera::calculateViewMatrix()const
{
	//eye: where the camera (a point)
	//center: where the camera point at (a point) if the point is unknown can be eye + direction
	//up: up vector of the camera (usually upward/positive y)
	return glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
}

glm::mat4 Camera::calculateProjectionMatrix(float FOVDegree, float aspect, float nearPlane, float farPlane) const
{
	return glm::perspective(FOVDegree, aspect, nearPlane, farPlane);
}

void Camera::processInputs(GLFWwindow* window, int windowWidth, int windowHeight, float deltaTime)
{
	/*Key inputs:
	A-D: x-axis
	W-S: z-axis
	SPACE-LEFT_CTRL: y-axis	
	*/
	//use multiple if instead of else-if so we can detect more than one keys pressed
	//NOTICE: glfwGetKey only returns GLFW_PRESS or GLFW_RELEASE but NOT!!! GLFW_REPEAT (when key is helddown)
	if(glfwGetKey(window, GLFW_KEY_W) ==  GLFW_PRESS)
	{
		m_Position += m_Speed * m_Direction * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_Position += m_Speed * -m_Direction * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_Position += m_Speed * -glm::normalize(glm::cross(m_Direction, m_Up)) * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//cross prod of (0.0, 0.0, -1.0) and (0.0, 1.0, 0.0) = (1.0, 0.0, 0.0)
		m_Position += m_Speed * glm::normalize(glm::cross(m_Direction, m_Up)) * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_Position += m_Speed * m_Up * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		m_Position += m_Speed * -m_Up * deltaTime;
	}

	/*Mouse inputs*/
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		//hide cursor if left mouse is pressed
		//GLFW_CURSOR_DISABLED creates virtual mouse
		//virutal mouse coords != real mouse coords
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//using cursor position to calculate yaw and pitch => rotate camera
		double currentMouseX;
		double currentMouseY;
		glfwGetCursorPos(window, &currentMouseX, &currentMouseY);
		//prevent camera jump
		if (m_isFirstMouse)
		{
			m_lastMouseX = currentMouseX;
			m_lastMouseY = currentMouseY;
			m_isFirstMouse = false;
		}
		m_Yaw += (currentMouseX - m_lastMouseX) * m_Sensitivity;//x movement of the mouse resembles yaw rotation
		m_Pitch += (m_lastMouseY - currentMouseY) * m_Sensitivity;//y = 0 at the top
		m_lastMouseX = currentMouseX;
		m_lastMouseY = currentMouseY;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (m_Pitch > 89.0f) m_Pitch = 89.0f;
		else if (m_Pitch < -89.0f) m_Pitch = -89.0f;

		//update vector up and direction
		updateCameraVectors();

	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		
		//show cursor if left mouse is realsed
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_isFirstMouse = true;
		
	}
}

void Camera::updateCameraVectors()
{
	//recalculate direction vector
	glm::vec3 newDir;
	newDir.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	newDir.y = sin(glm::radians(m_Pitch));
	newDir.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Direction = glm::normalize(newDir);
	
	//recalculate up vector
	glm::vec3 right = glm::normalize(glm::cross(m_Direction, DEFAULT_CAM_UP));
	m_Up = glm::normalize(glm::cross(right, m_Direction));
}
