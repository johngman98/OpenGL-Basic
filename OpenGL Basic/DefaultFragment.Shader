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
	float quadraticCoef = 3.0f	;
	float linearCoef = 0.7f;
	float constant = 1.0f;
	float attenuation = 1.0f / (quadraticCoef * distance * distance + linearCoef * distance + constant);

	//ambient light
	float ambient = 0.20f;

	//diffuse light
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVector);//vector dir from object's pos -> light's pos
	float diffuse = max(dot(normal, lightDirection), 0.0f);//make sure its positive

	//specular light
	float specular = 0.0f; //init
	if (diffuse != 0.0f) 
	{ //to prevent light shine on the otherside if the lightsource is under
		float specularStrength = 0.5f;
		vec3 viewDirection = normalize(cameraPosition - currentPos);
		vec3 reflectDirection = reflect(-lightDirection, normal);//reflect 1st argument is from the light to object

		//halfway vector 
		vec3 halfwayVec = normalize(viewDirection + reflectDirection);

		//if angle of view and reflect vectors > 90 -> dot prod < 0 -> max = 0
		//use halfway vector to prevent cutoff
		float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
		specular = specAmount * specularStrength;
	}
	return (texture(diffuse0, texCoord) * (ambient + diffuse * attenuation) + (texture(specular0, texCoord).r) * specular * attenuation) * lightColor;
}
vec4 genDirectionalLight()
{
	/*lightDir points from light to object and is normalized*/
	//ambient light
	float ambient = 0.20f;

	//diffuse light
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));//vector dir from object's pos -> light's pos
	float diffuse = max(dot(normal, lightDirection), 0.0f);//make sure its positive

	//specular light
	float specular = 0.0f; //init
	if (diffuse != 0.0f)
	{ //to prevent light shine on the otherside if the lightsource is under
		float specularStrength = 0.5f;
		vec3 viewDirection = normalize(cameraPosition - currentPos);
		vec3 reflectDirection = reflect(-lightDirection, normal);//reflect 1st argument is from the light to object

		//halfway vector 
		vec3 halfwayVec = normalize(viewDirection + reflectDirection);

		//if angle of view and reflect vectors > 90 -> dot prod < 0 -> max = 0
		//use halfway vector to prevent cutoff
		float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
		specular = specAmount * specularStrength;
	}

	return (texture(diffuse0, texCoord) * (ambient + diffuse) + (texture(specular0, texCoord).r) * specular ) * lightColor;
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
	float specular = 0.0f; //init
	if (diffuse != 0.0f)
	{ //to prevent light shine on the otherside if the lightsource is under
		float specularStrength = 0.5f;
		vec3 viewDirection = normalize(cameraPosition - currentPos);
		vec3 reflectDirection = reflect(-lightDirection, normal);//reflect 1st argument is from the light to object

		//halfway vector 
		vec3 halfwayVec = normalize(viewDirection + reflectDirection);

		//if angle of view and reflect vectors > 90 -> dot prod < 0 -> max = 0
		//use halfway vector to prevent cutoff
		float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
		specular = specAmount * specularStrength;
	}

	//check to see if the fragment will be lit
	float theta = dot(-lightDirection, vec3(0.0f, -1.0f, 0.0f));//angle between -lightDir and -Oy
	float intensity = clamp((theta - cutOff) / (1.0f - cutOff), 0.0f, 1.0f); 
	return (texture(diffuse0, texCoord) * (ambient + diffuse * intensity) + (texture(specular0, texCoord).r) * specular * intensity) * lightColor;
}

//should be the same as near and far planes
float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	/*
	Depth value is non linear: 
		F_depth = (1 / z - 1 / near) / (1 / far - 1 / near)
	*/

	/*Linearize depth value to visualize*/
	//transform z-value [0, 1] back to NDC(normalized device coord) [-1, 1]
	float ndc = depth * 2.0 - 1.0;
	//apply inverse transformation to retreve linear depth value
	float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));
	return linearDepth;

}

float logisticDepth(float depth, float steepness = 0.5f, float offset = 5.0f)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main()
{
	//Output Light
	fragColor = genPointLight();

	//Visualize depth testing
	//screen space coords (viewport) can be accessed thru gl_FragCoord
	//x, y is screen space coords starts at bottom left, 
	//z value used to compare to depth buffer's content and goes from [0, 1]
	//float depth = linearizeDepth(gl_FragCoord.z) / far;//depth value [near, far] (view space) => normalize by dividing by far
	//fragColor = vec4(vec3(depth) / far), 1.0f);

	//Fog effect
	//float depth = logisticDepth(gl_FragCoord.z);
	//fragColor = genDirectionalLight() * (1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
}