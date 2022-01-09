#version 330 core

//inputs from vertex shader
//in vec3 color;
in vec2 texCoord;
in vec3 currentPos;
in vec3 Normal;

//outputs
out vec4 fragColor;

//texture sampler
uniform sampler2D diffuse0;
uniform sampler2D specular0;
//uniform sampler2D tex2;
uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

vec4 genDirectionalLight()
{
	/*lightDir points from light to object and is normalized*/
	//ambient light
	float ambient = 0.20f;

	//diffuse light
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));//vector dir from object's pos -> light's pos
	float diffuse = max(abs(dot(normal, lightDirection)), 0.0f);//abs helps grass shading

	//specular light
	float specularStrength = 0.5f;
	vec3 viewDirection = normalize(cameraPosition - currentPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);//reflect 1st argument is from the light to object
	float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
	float specular = specAmount * specularStrength;

	if (texture(diffuse0, texCoord).a < 0.1f)
		discard;

	return (texture(diffuse0, texCoord) * (ambient + diffuse) + (texture(specular0, texCoord).r) * specular) * lightColor;
}


void main()
{
	//Output Light
	//vec4 texColor = texture(diffuse0, texCoord) + texture(specular0, texCoord);
	//if (texColor.a < 0.1f)
		//discard;
	fragColor = genDirectionalLight();

}