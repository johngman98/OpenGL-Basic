#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 lightModel;
uniform mat4 view;
uniform mat4 lightProj;

void main()
{
	gl_Position = lightProj * view * lightModel * vec4(aPos, 1.0f);
}
