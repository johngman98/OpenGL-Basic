#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;

//mvp
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

//transformations
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

out DATA
{
	vec3 Normal;
	vec2 texCoord;
	mat4 proj;
} data_out;

void main()
{
	gl_Position = model * translation * rotation * scale * vec4(aPos, 1.0f);
	data_out.Normal = aNormal;
	data_out.texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
	data_out.proj = proj * view;
}