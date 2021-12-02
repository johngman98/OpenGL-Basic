#include "Shader.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	//aware of dangling pointers if use c_str directly
	std::string vertextCode = getFileContent(vertexPath);
	std::string fragmentCode = getFileContent(fragmentPath);
	const char* vertextSource = vertextCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertextSource, NULL);
	glCompileShader(vertexShaderID);
	//get errors log
	compileErrors(vertexShaderID, "VERTEX_SHADER");

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShaderID);
	compileErrors(fragmentShaderID, "FRAGMENT_SHADER");

	m_ProgramID = glCreateProgram();
	glAttachShader(m_ProgramID, vertexShaderID);
	glAttachShader(m_ProgramID, fragmentShaderID);
	glLinkProgram(m_ProgramID);
	compileErrors(m_ProgramID, "PROGRAM");

	glDetachShader(m_ProgramID, vertexShaderID);
	glDetachShader(m_ProgramID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

}

Shader::~Shader()
{
	glDeleteProgram(m_ProgramID);
}

void Shader::bindProgram() const
{
	glUseProgram(m_ProgramID);
}

void Shader::unbindProgram() const
{
	glUseProgram(0);
}

GLuint Shader::getProgramID()
{
	return m_ProgramID;
}

void Shader::setUniform1i(const std::string& uniformName, GLint value) const
{
	glUniform1i(getUniformLocation(uniformName), value);
}

void Shader::setUniformMatrix4fv(const std::string& uniformName, const glm::mat4& matrix) const
{
	//float matrix 4x4
	glUniformMatrix4fv(getUniformLocation(uniformName), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setUniform4f(const std::string& uniformName, glm::vec4 vector) const
{
	glUniform4f(getUniformLocation(uniformName), vector.x, vector.y, vector.z, vector.w);
}

void Shader::setUniform3f(const std::string& uniformName, glm::vec3 vector) const
{
	glUniform3f(getUniformLocation(uniformName), vector.x, vector.y, vector.z);
}

std::string Shader::getFileContent(const std::string& filePath)
{
	const std::string ERR_MSG = "Failed to read file: " + filePath;
	std::string fileContent = "";
	std::string line = "";
	std::fstream fileStream(filePath);
	
	//check state of the stream, only return true if none of the error flags is set (eofbit, failbit, badbit)
	if(!fileStream.good())
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

void Shader::compileErrors(GLuint ID, const std::string& type)
{
	GLint hasCompiled;
	char infoLog[1024];
	if(type != "PROGRAM")
	{
		glGetShaderiv(ID, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(ID, 1024, NULL, infoLog);
			std::cout << type << "_COMPILATION_ERROR :" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(ID, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(ID, 1024, NULL, infoLog);
			std::cout << type << "_LINKING_ERROR :" << infoLog << std::endl;
		}
	}
}

GLint Shader::getUniformLocation(const std::string& uniformName) const
{

	GLint location = glGetUniformLocation(m_ProgramID, uniformName.c_str());
	if (location == -1)
		//uniform that is unused by the complier with be discard, hence location = -1
		std::cout << "Uniform varibale with name: " << uniformName << " does not exist or use"<< std::endl;
	return location;
}

