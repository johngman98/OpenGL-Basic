#include "Model.h"

Model::Model(const std::string& filePath)
{
	std::string content = getFileContent(filePath);
	m_JSON = json::parse(content);

	m_FilePath = filePath;
	m_Data = getData();

	traverseNode(0);
}

Model::~Model()
{
}

void Model::draw(const Shader& shader, const Camera& camera) 
{
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].draw(shader, camera, m_MatricesMeshes[i]);
	}
}
std::vector<unsigned char> Model::getData() const
{
	std::string bytesText;
	std::string	uri = m_JSON["buffers"][0]["uri"];
	std::string fileDirectory = m_FilePath.substr(0, m_FilePath.find_last_of('/') + 1);
	bytesText = get_file_contents((fileDirectory + uri).c_str());

	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;

}

std::vector<float> Model::getFloats(const json& accessor) const
{
	std::vector<float> floatVec;

	unsigned int bufferViewIndex = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	json bufferView = m_JSON["bufferViews"][bufferViewIndex];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int numFloatsPerVert;
	if (type == "SCALAR") numFloatsPerVert = 1;
	else if (type == "VEC2") numFloatsPerVert = 2;
	else if (type == "VEC3") numFloatsPerVert = 3;
	else if (type == "VEC4") numFloatsPerVert = 4;
	else throw std::invalid_argument("Type is not valid (not SCALAR, VEC2, VEC3, VEC4");

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

	unsigned int bufferViewIndex = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	json bufferView = m_JSON["bufferViews"][bufferViewIndex];
	unsigned int byteOffset = bufferView["byteOffset"];

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

	for (size_t i = 0; i < m_JSON["images"].size(); i++)
	{
		std::string texturePath = m_JSON["images"][i]["uri"];

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
		if (!skip) 
		{
			if (texturePath.find("baseColor") != std::string::npos)
			{
				Texture diffuse = Texture(fileDir + texturePath, "diffuse");
				textures.push_back(diffuse);
				m_LoadedTextures.push_back(diffuse);
				m_LoadedTextureNames.push_back(texturePath);
			}
			else if (texturePath.find("metallicRoughness") != std::string::npos)
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
	std::vector<Vertex> vertices;
	for(size_t i = 0; i < positions.size(); i++)
	{
		vertices.push_back(Vertex{ positions[i], normals[i], textureCoordinates[i] });
	}
	return vertices;
}

void Model::loadMesh(unsigned int meshIndex)
{
	unsigned int posAccInd = m_JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normAccInd = m_JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = m_JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = m_JSON["meshes"][meshIndex]["primitives"][0]["indices"];

	std::vector<float> posVec = getFloats(m_JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = groupFloatsVec3(posVec);

	std::vector<float> normVec = getFloats(m_JSON["accessors"][normAccInd]);
	std::vector<glm::vec3> normals = groupFloatsVec3(normVec);

	std::vector<float> texCoordVec = getFloats(m_JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> textureCoords = groupFloatsVec2(texCoordVec);

	std::vector<Vertex> vertices = assembleVertices(positions, normals, textureCoords);
	std::vector<GLuint> indices = getIndices(m_JSON["accessors"][indAccInd]);
	std::vector<Texture> textures = getTexture();
	
	m_Meshes.push_back(Mesh(vertices, indices, textures));
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
	json node = m_JSON["nodes"][nextNode];

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

	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		float rotValues[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2],
		};
		rotation = glm::make_quat(rotValues);
	}

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

	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	if (node.find("mesh") != node.end())
	{
		m_TranslationMeshes.push_back(translation);
		m_RotationsMeshes.push_back(rotation);
		m_ScalesMeshes.push_back(scale);
		m_MatricesMeshes.push_back(matNextNode);

		loadMesh(node["mesh"]);
	}

	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
		{
			traverseNode(node["children"][i], matNextNode);
		}
	}
}





