#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;


//out vec3 color;
out vec2 texCoord;
out vec3 Normal; //in world sapce
out vec3 currentPos;//in world space
out vec4 fragPositionLight;// Outputs the fragment position of the light

//mvp
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

//transformations
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

uniform mat4 lightProjection;

void main()
{
	//maybe -rotation if model flips 
	currentPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
	gl_Position = proj * view *  vec4(currentPos, 1.0f); 
	//rotate tex coord by 90 degree
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
	// Calculates the position of the light fragment for the fragment shader
	fragPositionLight = lightProjection * vec4(currentPos, 1.0f);
	//this is ok because we didnt translate, rotate or sacle 
	Normal = aNormal;
}