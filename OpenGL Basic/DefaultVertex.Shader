#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;


//out vec3 color;
out vec2 texCoord;
out vec3 Normal; //in world sapce
out vec3 currentPos;//in world space


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	currentPos = vec3(model * vec4(aPos, 1.0f));
	gl_Position = proj * view *  vec4(currentPos, 1.0f); 
	//color = aColor;
	texCoord = aTex;
	
	
	//this is ok because we didnt translate, rotate or sacle 
	Normal = aNormal;
}