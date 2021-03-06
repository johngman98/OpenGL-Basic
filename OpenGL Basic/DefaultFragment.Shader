#version 330 core

//inputs from vertex shader
//in vec3 color;
in vec2 texCoord;
in vec3 currentPos;
in vec3 Normal;
in vec4 fragPositionLight;

//outputs
out vec4 fragColor;

//texture sampler
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D shadowMap;
uniform samplerCube shadowCubeMap;
//uniform sampler2D tex2;
uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform float farPlane;

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

	// Shadow value
	float shadow = 0.0f;
	vec3 fragToLight = currentPos - lightPosition;
	float currentDepth = length(fragToLight);
	float bias = max(0.5f * (1.0f - dot(normal, lightDirection)), 0.0005f);

	// Not really a radius, more like half the width of a square
	int sampleRadius = 2;
	float offset = 0.02f;
	for (int z = -sampleRadius; z <= sampleRadius; z++)
	{
		for (int y = -sampleRadius; y <= sampleRadius; y++)
		{
			for (int x = -sampleRadius; x <= sampleRadius; x++)
			{
				float closestDepth = texture(shadowCubeMap, fragToLight + vec3(x, y, z) * offset).r;
				// Remember that we divided by the farPlane?
				// Also notice how the currentDepth is not in the range [0, 1]
				closestDepth *= farPlane;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;
			}
		}
	}
	// Average shadow
	shadow /= pow((sampleRadius * 2 + 1), 3);

	return (texture(diffuse0, texCoord) * (ambient + diffuse * (1.0f - shadow) * attenuation) + (texture(specular0, texCoord).r) * specular * (1.0f - shadow) * attenuation) * lightColor;
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
	
		//halfway vector 
		vec3 halfwayVec = normalize(viewDirection + lightDirection);

		//if angle of view and reflect vectors > 90 -> dot prod < 0 -> max = 0
		//use halfway vector to prevent cutoff
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularStrength;
	}
	// Shadow value [0.0, 1.0]
	float shadow = 0.0f; 
	// Sets lightCoords to clipped space
	vec3 lightCoords = fragPositionLight.xyz / fragPositionLight.w;
	if (lightCoords.z <= 1.0f) //inside of the frustrum of orthographic proj
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float closestDepth = texture(shadowMap, lightCoords.xy).r;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.025f * (1.0f - dot(normal, lightDirection)), 0.0005f);
		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for (int y = -sampleRadius; y <= sampleRadius; y++)
		{
			for (int x = -sampleRadius; x <= sampleRadius; x++)
			{
				float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;
			}
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);
	}


	return (texture(diffuse0, texCoord) * (ambient + diffuse * (1.0f - shadow)) + (texture(specular0, texCoord).r) * specular * (1.0f - shadow)) * lightColor;
}
vec4 genSpotLight()
{	
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

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
	float theta = dot(vec3(0.0f, -1.0f, 0.0f), - lightDirection);//angle between -lightDir and -Oy
	float intensity = clamp((theta - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fragPositionLight.xyz / fragPositionLight.w;
	if (lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.00025f * (1.0f - dot(normal, lightDirection)), 0.000005f);

		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for (int y = -sampleRadius; y <= sampleRadius; y++)
		{
			for (int x = -sampleRadius; x <= sampleRadius; x++)
			{
				float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;
			}
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);

	}

	return (texture(diffuse0, texCoord) * (ambient + diffuse * (1 - shadow) * intensity) + (texture(specular0, texCoord).r) * specular * (1 - shadow) * intensity) * lightColor;
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