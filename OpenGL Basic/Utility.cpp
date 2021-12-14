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
