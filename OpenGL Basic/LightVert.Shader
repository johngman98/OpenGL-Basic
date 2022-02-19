#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

//transformations
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;


void main()
{
	vec3 currentPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
	gl_Position = proj * view * vec4(currentPos, 1.0f);
}
