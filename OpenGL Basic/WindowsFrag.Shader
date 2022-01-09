#version 330 core

out vec4 fragColor;
in vec2 texCoord;

uniform sampler2D diffuse0;

void main()
{
	// discards all fragments with alpha less than 0.1
	if (texture(diffuse0, texCoord).a < 0.1)
		discard;
	// outputs final color
	fragColor = texture(diffuse0, texCoord);
}