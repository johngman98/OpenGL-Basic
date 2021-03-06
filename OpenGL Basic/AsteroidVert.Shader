#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in mat4 instanceMatrix;


//out vec3 color;
out vec2 texCoord;
out vec3 Normal; //in world sapce
out vec3 currentPos;//in world space

//mvp
uniform mat4 view;
uniform mat4 proj;


void main()
{
	//maybe -rotation if model flips 
	currentPos = vec3(instanceMatrix * vec4(aPos, 1.0f));
	gl_Position = proj * view * vec4(currentPos, 1.0f);
	//color = aColor;
	//rotate tex coord by 90 degree
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;


	//this is ok because we didnt translate, rotate or sacle 
	Normal = aNormal;
}