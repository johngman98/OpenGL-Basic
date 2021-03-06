#pragma once

#include <json.h>
#include "Utility.h"
#include "Mesh.h"

using json = nlohmann::json;

class Model
{
public:
	Model(const std::string& filePath, unsigned int numInstances = 1, const std::vector<glm::mat4>& instanceMatrices = {});
	~Model();

	void draw(const Shader& shader, const Camera& camera);
	void draw(const Shader& shader, const Camera& camera,
			  glm::vec3 translation);

private:
	std::vector<unsigned char> getData() const;
	std::vector<float> getFloats(const json& accessor) const;
	std::vector<GLuint> getIndices(const json& accessor) const;
	std::vector<Texture> getTexture();

	std::vector<glm::vec2> groupFloatsVec2(const std::vector<float>& floatVec) const;
	std::vector<glm::vec3> groupFloatsVec3(const std::vector<float>& floatVec) const;
	std::vector<glm::vec4> groupFloatsVec4(const std::vector<float>& floatVec) const;

	std::vector<Vertex> assembleVertices(const std::vector<glm::vec3>& positions,
										 const std::vector<glm::vec3>& normals,
										 const std::vector<glm::vec2>& textureCoordinates) const;

	void loadMesh(unsigned int meshIndex);
	void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

private:
	std::string m_FilePath;
	std::vector<unsigned char> m_Data;
	json m_JSON;
	unsigned int m_numIntances;

	//to keep track of which texture was loaded
	std::vector<std::string> m_LoadedTextureNames;
	std::vector<Texture> m_LoadedTextures;

	std::vector<Mesh> m_Meshes;

	//tranformation matrices
	std::vector<glm::vec3> m_TranslationMeshes;
	std::vector<glm::quat> m_RotationsMeshes;
	std::vector<glm::vec3> m_ScalesMeshes;
	std::vector<glm::mat4> m_MatricesMeshes;
	std::vector<glm::mat4> m_InstanceMatrices;

};