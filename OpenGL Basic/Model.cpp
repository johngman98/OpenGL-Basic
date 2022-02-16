#include "Model.h"

Model::Model(const std::string& filePath, unsigned int numInstances, const std::vector<glm::mat4>& instanceMatrices)
	:m_numIntances(numInstances), m_InstanceMatrices(instanceMatrices)
{
	/*Createand store a json object(how to interpret data)*/
	std::string content = getFileContent(filePath);
	m_JSON = json::parse(content);

	/*Get the binary file(real data)*/
	m_FilePath = filePath;
	m_Data = getData();

	/*Traverse all the nodes*/
	traverseNode(0);
}

Model::~Model()
{
}

void Model::draw(const Shader& shader, const Camera& camera) 
{
	/*Go over all the meshes draw*/
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].draw(shader, camera, m_MatricesMeshes[i]);
	}
}
void Model::draw(const Shader& shader, const Camera& camera, glm::vec3 translation)
{
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].Mesh::draw(shader, camera, m_MatricesMeshes[i], translation);
	}
}
std::vector<unsigned char> Model::getData() const
{
	/*Create a place to store raw textand get the uri(path) of the binary file*/
	std::string bytesText;
	std::string	uri = m_JSON["buffers"][0]["uri"];

	/*Extractand store bin file content into a string object*/
	std::string fileDirectory = m_FilePath.substr(0, m_FilePath.find_last_of('/') + 1);
	bytesText = get_file_contents((fileDirectory + uri).c_str());

	/*Transform raw text data into bytesand put them into a vector*/
	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;

}

std::vector<float> Model::getFloats(const json& accessor) const
{
	std::vector<float> floatVec;

	/*Get properties from accessor*/
	//"bufferView" and "byteOffset" dont always exist so accesor.value using take of []
	//that way if the keys dont exist it takes the second argument as value
	unsigned int bufferViewIndex = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	/*Get properties from bufferViews*/
	json bufferView = m_JSON["bufferViews"][bufferViewIndex];
	unsigned int byteOffset = bufferView["byteOffset"];

	/*Interpret type and assgin numFloatsPerVert accordingly*/
	unsigned int numFloatsPerVert = 0;
	if (type == "SCALAR") numFloatsPerVert = 1;
	else if (type == "VEC2") numFloatsPerVert = 2;
	else if (type == "VEC3") numFloatsPerVert = 3;
	else if (type == "VEC4") numFloatsPerVert = 4;
	else throw std::invalid_argument("Type is not valid (not SCALAR, VEC2, VEC3, VEC4");

	/*Go over all the bytes in the data at the correct place using the properties from above*/
	unsigned int beginningOfData = byteOffset + accessorByteOffset;
	unsigned int lengthOfData = count * 4 * numFloatsPerVert;
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData;)
	{
		unsigned char bytes[] = { m_Data[i++], m_Data[i++], m_Data[i++], m_Data[i++] };
		float value = 0.0f;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}
	return floatVec;
}

std::vector<GLuint> Model::getIndices(const json& accessor) const
{
	std::vector<GLuint> indices;

	/*Get properties from the accessor*/
	unsigned int bufferViewIndex = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	/*Get properties from bufferViews*/
	json bufferView = m_JSON["bufferViews"][bufferViewIndex];
	unsigned int byteOffset = bufferView["byteOffset"];

	/*Get indices with regards to their type: unsigned int, unsigned short, or short*/
	unsigned int beginningOfData = byteOffset + accessorByteOffset;
	if (componentType == 5125)
	{
		for(unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 4;)
		{
			unsigned char bytes[] = { m_Data[i++], m_Data[i++], m_Data[i++], m_Data[i++] };
			unsigned int value = 0;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}

	else if(componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2;)
		{
			unsigned char bytes[] = { m_Data[i++], m_Data[i++] };
			unsigned short value = 0;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if(componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2;)
		{
			unsigned char bytes[] = { m_Data[i++], m_Data[i++] };
			short value = 0;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}
	return indices;
}

std::vector<Texture> Model::getTexture() 
{
	std::vector<Texture> textures;

	std::string fileDir = m_FilePath.substr(0, m_FilePath.find_last_of('/') + 1);

	/*Iterate over all images*/
	for (size_t i = 0; i < m_JSON["images"].size(); i++)
	{
		//uri of current texture
		std::string texturePath = m_JSON["images"][i]["uri"];
		
		//Check if the texture has already been loaded
		bool skip = false;
		for(size_t j = 0; j < m_LoadedTextureNames.size(); j++)
		{
			if(m_LoadedTextureNames[j] == texturePath)
			{
				textures.push_back(m_LoadedTextures[j]);
				skip = true;
				break;
			}
		}

		//If the texture has been loaded, skip this
		if (!skip) 
		{
			//diffuse texture
			if (texturePath.find("baseColor") != std::string::npos || texturePath.find("diffuse") != std::string::npos)
			{
				Texture diffuse = Texture(fileDir + texturePath, "diffuse");
				textures.push_back(diffuse);
				m_LoadedTextures.push_back(diffuse);
				m_LoadedTextureNames.push_back(texturePath);
			}
			//specular texture
			else if (texturePath.find("metallicRoughness") != std::string::npos || texturePath.find("specular") != std::string::npos)
			{
				Texture specular = Texture(fileDir + texturePath, "specular");
				textures.push_back(specular);
				m_LoadedTextures.push_back(specular);
				m_LoadedTextureNames.push_back(texturePath);
			}
		}
	}
	return textures;
}


std::vector<glm::vec2> Model::groupFloatsVec2(const std::vector<float>& floatVec) const
{
	std::vector<glm::vec2> floatVec2s;
	for(size_t i = 0; i < floatVec.size();)
	{
		floatVec2s.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
	}
	return floatVec2s;
}

std::vector<glm::vec3> Model::groupFloatsVec3(const std::vector<float>& floatVec) const
{
	std::vector<glm::vec3> floatVec3s;
	for(size_t i = 0; i < floatVec.size();)
	{
		floatVec3s.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return floatVec3s;
}

std::vector<glm::vec4> Model::groupFloatsVec4(const std::vector<float>& floatVec) const
{
	std::vector<glm::vec4> floatVec4s;
	for (size_t i = 0; i < floatVec.size();)
	{
		floatVec4s.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return floatVec4s;
}

std::vector<Vertex> Model::assembleVertices(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& textureCoordinates) const
{
	/*Putting all data following Vertex layouts: pos, norm, uv*/
	std::vector<Vertex> vertices;
	for(size_t i = 0; i < positions.size(); i++)
	{
		vertices.push_back(Vertex{ positions[i], normals[i], textureCoordinates[i] });
	}
	return vertices;
}

void Model::loadMesh(unsigned int meshIndex)
{
	/*Get all accessor indices*/
	unsigned int posAccInd = m_JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normAccInd = m_JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = m_JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = m_JSON["meshes"][meshIndex]["primitives"][0]["indices"];

	/*Using the above indices to get vertices components*/
	//position
	std::vector<float> posVec = getFloats(m_JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
	//normal
	std::vector<float> normVec = getFloats(m_JSON["accessors"][normAccInd]);
	std::vector<glm::vec3> normals = groupFloatsVec3(normVec);
	//texture coordinate
	std::vector<float> texCoordVec = getFloats(m_JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> textureCoords = groupFloatsVec2(texCoordVec);
	//Assemle components
	std::vector<Vertex> vertices = assembleVertices(positions, normals, textureCoords);
	//Get indices
	std::vector<GLuint> indices = getIndices(m_JSON["accessors"][indAccInd]);
	//Get textures
	std::vector<Texture> textures = getTexture();
	
	/*Create and store mesh*/
	m_Meshes.push_back(Mesh(vertices, indices, textures, m_numIntances, m_InstanceMatrices));
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
	/*Current node*/
	json node = m_JSON["nodes"][nextNode];

	/*Get all the transformations and model matrix if exist*/
	//Translation
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end())
	{
		float transValues[3] = { 0.0f };
		for (unsigned int i = 0; i < node["translation"].size(); i++)
		{
			transValues[i] = (node["translation"][i]);
		}
		translation = glm::make_vec3(transValues);
	}
	//Rotation
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		float rotValues[4] =
		{	
			//standardization conflict between glm and gltf
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2],
		};
		rotation = glm::make_quat(rotValues);
	}
	//Scale
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end())
	{
		float scaleValues[3] = { 0.0f };
		for (unsigned int i = 0; i < node["scale"].size(); i++)
		{
			scaleValues[i] = (node["scale"][i]);
		}
		scale = glm::make_vec3(scaleValues);

	}
	//Model matrix of the mesh
	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matValues[16] = { 0.0f };
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
		{
			matValues[i] = (node["matrix"][i]);
		}
		matNode = glm::make_mat4(matValues);
	}

	//Init matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);
	//Using the transformations above
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);
	//Calculate the current model matrix with all the transformations
	//"matrix" is the model matrix of the parent of this current node
	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	//Check if the node contains a meshand if it does load it
	if (node.find("mesh") != node.end())
	{
		m_TranslationMeshes.push_back(translation);
		m_RotationsMeshes.push_back(rotation);
		m_ScalesMeshes.push_back(scale);
		m_MatricesMeshes.push_back(matNextNode);

		loadMesh(node["mesh"]);
	}
	//Check if the node has children
	//if it does, the current model matrix will be the input for the children node
	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
		{
			traverseNode(node["children"][i], matNextNode);
		}
	}
}





