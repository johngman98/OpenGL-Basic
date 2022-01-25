#include "Shader.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
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

	//geometry shader
	GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	if (!geometryPath.empty()) 
	{
		std::string geometryCode = getFileContent(geometryPath);
		const char* geometrySource = geometryCode.c_str();
		glShaderSource(geometryShaderID, 1, &geometrySource, NULL);
		glCompileShader(geometryShaderID);
		compileErrors(geometryShaderID, "GEOMETRY_SHADER");
	}
	m_ProgramID = glCreateProgram();
	glAttachShader(m_ProgramID, vertexShaderID);
	glAttachShader(m_ProgramID, fragmentShaderID);
	if (!geometryPath.empty()) glAttachShader(m_ProgramID, geometryShaderID);
	glLinkProgram(m_ProgramID);
	compileErrors(m_ProgramID, "PROGRAM");

	glDetachShader(m_ProgramID, vertexShaderID);
	glDetachShader(m_ProgramID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	if (!geometryPath.empty())
	{
		glDetachShader(m_ProgramID, geometryShaderID);
		glDeleteShader(geometryShaderID);
	}

}

Shader::~Shader()
{
	//be very aware of this
	//glDeleteProgram(m_ProgramID);
}

void Shader::bindProgram() const
{
	glUseProgram(m_ProgramID);
}

void Shader::unbindProgram() const
{
	glUseProgram(0);
}

void Shader::deleteProgram() const
{
	glDeleteProgram(m_ProgramID);
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

void Shader::setUniform1f(const std::string& uniformName, float value) const
{
	glUniform1f(getUniformLocation(uniformName), value);
}

void Shader::compileErrors(GLuint ID, const std::string& type) const
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
	//if (location == -1)
		//uniform that is unused by the complier with be discard, hence location = -1
		//std::cout << "Uniform varibale with name: " << uniformName << " does not exist or use"<< std::endl;
	return location;
}

