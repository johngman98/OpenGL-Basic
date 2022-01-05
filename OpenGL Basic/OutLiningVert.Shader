#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

//mvp
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

//transformations
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

//outlining
//uniform float outlining;

void main()
{
	//using normal to scale
	//vec3 currentPos = vec3(model * translation * rotation * scale  * vec4(aPos + aNormal * outlining, 1.0f));

	//using bigger model
	vec3 currentPos = vec3(model * translation * rotation * scale * vec4(aPos, 0.0f));
	gl_Position = proj * view * vec4(currentPos, 1.0f);
}