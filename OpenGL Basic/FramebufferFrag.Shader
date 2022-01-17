#version 330 core

out vec4  FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;


//offset can be customeized
const float offset_x = 1.0f / 800.0f; //width 
const float offset_y = 1.0f / 800.0f; //height

//current pixel's neighbor
vec2 offsets[9] = vec2[](
	vec2(-offset_x, offset_y),   //top_left
	vec2(0.0f, offset_y),		 //top_center
	vec2(offset_x, offset_y),    //top_right
	vec2(-offset_x, 0.0f),       //center_left
	vec2(0.0f, 0.0f),            //center_center (current pixel)
	vec2(offset_x, 0.0f),        //center_right
	vec2(-offset_x, -offset_y),  //bottom_left
	vec2(0.0f, -offset_y),       //bottom_center
	vec2(offset_x, -offset_y)    //bottem_right
	);
//kernel = matrix of weights of each pixel, pixel of interest is in the center
float kernel[9] = float[](
	1, 1, 1,
	1, -8, 1,
	1, 1, 1
	);



void main()
{
	
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++)
		color += vec3(texture(screenTexture, texCoords.st + offsets[i])) * kernel[i];
	FragColor = vec4(color, 1.0f);
}