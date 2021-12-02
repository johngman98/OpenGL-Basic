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

vec4 genPointLight()
{
	vec3 lightVector = lightPosition - currentPos;
	float distance = length(lightVector);
	float quadraticCoef = 0.032f;
	float linearCoef = 0.09f;
	float constant = 1.0f;
	float attenuation = 1.0f / (quadraticCoef * distance * distance + linearCoef * distance + constant);

	//ambient light
	float ambient = 0.20f;

	//diffuse light
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVector);//vector dir from object's pos -> light's pos
	float diffuse = max(dot(normal, lightDirection), 0.0f);//make sure its positive

	//specular light
	float specularStrength = 0.5f;
	vec3 viewDirection = normalize(cameraPosition - currentPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);//reflect 1st argument is from the light to object
	float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
	float specular = specAmount * specularStrength;

	return (texture(diffuse0, texCoord) * (ambient + diffuse * attenuation) + (texture(specular0, texCoord).r) * specular * attenuation) * lightColor;
}
vec4 genDirectionalLight(vec3 lightDir)
{
	/*lightDir points from light to object and is normalized*/
	//ambient light
	float ambient = 0.20f;

	//diffuse light
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(-lightDir);//vector dir from object's pos -> light's pos
	float diffuse = max(dot(normal, lightDirection), 0.0f);//make sure its positive

	//specular light
	float specularStrength = 0.5f;
	vec3 viewDirection = normalize(cameraPosition - currentPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);//reflect 1st argument is from the light to object
	float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
	float specular = specAmount * specularStrength;

	return (texture(diffuse0, texCoord) * (ambient + diffuse ) + (texture(specular0, texCoord).r) * specular ) * lightColor;
}
vec4 genSpotLight(float cutOff)
{	
	vec3 lightVector = lightPosition - currentPos;

	//ambient light
	float ambient = 0.20f;

	//diffuse light
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVector);//vector dir from object's pos -> light's pos
	float diffuse = max(dot(normal, lightDirection), 0.0f);//make sure its positive

	//specular light
	float specularStrength = 0.5f;
	vec3 viewDirection = normalize(cameraPosition - currentPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);//reflect 1st argument is from the light to object
	float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
	float specular = specAmount * specularStrength;

	//check to see if the fragment will be lit
	float theta = dot(-lightDirection, vec3(0.0f, -1.0f, 0.0f));//angle between -lightDir and -Oy
	float intensity = clamp((theta - cutOff) / (1.0f - cutOff), 0.0f, 1.0f); 
	return (texture(diffuse0, texCoord) * (ambient + diffuse * intensity) + (texture(specular0, texCoord).r) * specular * intensity) * lightColor;
}

void main()
{
	fragColor = genPointLight();
}