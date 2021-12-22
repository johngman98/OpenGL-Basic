#include "Utility.h"

std::string getFileContent(const std::string& filePath)
{
	const std::string ERR_MSG = "Failed to read file: " + filePath;
	std::string fileContent = "";
	std::string line = "";
	std::fstream fileStream(filePath);

	//check state of the stream, only return true if none of the error flags is set (eofbit, failbit, badbit)
	if (!fileStream.good())
	{
		/*TODO: output useful error logs*/
		std::cout << ERR_MSG << std::endl;
		return ERR_MSG;
	}

	//extract content
	while (!fileStream.eof())//endoffile
	{
		std::getline(fileStream, line);
		fileContent += line + "\n";
	}
	return fileContent;
}

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}


std::vector<Vertex> loadObj(const std::string& filePath)
{
	std::fstream fileStream(filePath);
	/*check state of the file stream*/
	if (!fileStream.good())
		std::cout << "File not found!!" << std::endl;

	/*extract file content*/
	//temporary line
	std::string line;
	//to store actual data
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoordinates;
	//to store which indices to used
	std::vector<GLuint> posIndices;
	std::vector<GLuint> normIndices;
	std::vector<GLuint> texCoordIndices;
	while (!fileStream.eof())
	{
		//read file stream and store in line
		std::getline(fileStream, line);

		//get first two chars
		std::string prefix = line.substr(0, 2);
		//position data
		if (prefix == "v ")
		{
			std::stringstream stream(line.substr(2));
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			stream >> x >> y >> z;
			positions.push_back(glm::vec3(x, y, z));
		}
		//normal data
		else if (prefix == "vn")
		{
			std::stringstream stream(line.substr(2));
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			stream >> x >> y >> z;
			normals.push_back(glm::vec3(x, y, z));
		}
		//texture coordinate data
		else if (prefix == "vt")
		{
			std::stringstream stream(line.substr(2));
			float x = 0.0f;
			float y = 0.0f;
			stream >> x >> y;
			textureCoordinates.push_back(glm::vec2(x, y));
		}
		//face => indices
		else if (prefix == "f ")
		{
			std::stringstream stream(line.substr(2));
			std::string vert;
			//each line of face contain 3 vertices: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
			while (stream >> vert)
			{
				size_t pos1 = vert.find_first_of('/');
				size_t pos2 = vert.find_first_of('/', pos1 + 1);


				posIndices.push_back(std::stoul(vert.substr(0, pos1)));
				texCoordIndices.push_back(std::stoul(vert.substr(pos1 + 1, pos2 - pos1 - 1)));
				normIndices.push_back(std::stoul(vert.substr(pos2 + 1)));

				/*TODO: handle where missing postion, normal or uv coord*/
			}
		}

	}

	/*Populate the vertices vector w.r.t indices*/
	std::vector<Vertex> result;
	//size of posIndices, texCoordIndices and normIndices should be equal
	result.reserve(posIndices.size());
	for (size_t i = 0; i < posIndices.size(); i++)
	{
		//obj format: index starts at 1
		glm::vec3 pos = positions[posIndices[i] - 1];
		glm::vec3 norm = normals[normIndices[i] - 1];
		glm::vec2 texCoord = textureCoordinates[texCoordIndices[i] - 1];
		result.push_back({ pos, norm, texCoord });

	}

	/*for(auto v: result)
	{
		std::cout << "pos: " << v.position.x << v.position.y << v.position.z << std::endl;
		std::cout << "norm: " << v.normal.x << v.normal.y << v.normal.z << std::endl;
		std::cout << "tex: " << v.textureUV.x << v.textureUV.y << std::endl;
	}*/
	return result;
}
