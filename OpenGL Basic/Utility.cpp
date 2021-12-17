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
