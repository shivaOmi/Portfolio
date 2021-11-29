#include<iostream>
#include<fstream>
#include<sstream>
#include "Shader.h"
#include <GL/glew.h>

enum class ShaderType {
	NONE=-1,
	VERTEX=0,
	FRAGMENT=1
};

Shader::Shader(const std::string& filepath) :m_vertexShaderSource(""), m_fragmentShaderSource(""),m_programID(0) {
	std::ifstream stream(filepath);
	std::string line;
	ShaderType type = ShaderType::NONE;
	std::stringstream ss[2];

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {																	
			ss[(int)type] << line << '\n';
		}
	}

	m_vertexShaderSource = ss[(int)ShaderType::VERTEX].str();
	m_fragmentShaderSource = ss[(int)ShaderType::FRAGMENT].str();
}

void Shader::Bind() {
	glUseProgram(Shader::m_programID);
}

void Shader::Unbind() {
	glUseProgram(0);
}

unsigned int Shader::CreateProgram(const std::string& vertexSource, const std::string& fragmentSource) {
	const char* vertexSrc = vertexSource.c_str();
	const char* fragmentSrc = fragmentSource.c_str();

	m_programID = glCreateProgram();

	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	glAttachShader(m_programID, vs);
	glAttachShader(m_programID, fs);

	glLinkProgram(m_programID);

	glDetachShader(m_programID, vs);
	glDeleteShader(vs);
	glDetachShader(m_programID, fs);
	glDeleteShader(fs);

	return Shader::m_programID;
}

int Shader::GetUniformLocation(const std::string& name) {
	int location = glGetUniformLocation(m_programID, name.c_str());
	assert(location != -1);
	return location;
}

void Shader::SetUniform1i(const std::string& name, int value) {
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value) {
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3 &v){
	glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(v));
}

void Shader::SetUniformMatrix4fv(const std::string& name, glm::mat4& value) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& shaderSource) {
	unsigned int id = glCreateShader(type);

	auto shaderSrc = shaderSource.c_str();

	glShaderSource(id, 1, &shaderSrc, nullptr);
	glCompileShader(id);

	int iInfoLogLength = 0;
	int iShaderCompileStatus = 0;
	char* szLogInfo = NULL;
	glGetShaderiv(id, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szLogInfo = (char*)malloc(iInfoLogLength);
			if (szLogInfo != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(id, iInfoLogLength, &written, szLogInfo);
				std::cout<< (type == GL_VERTEX_SHADER?"Vertex":"Fragment")<< "Shader Compilation Log : %s\n" << szLogInfo;
				free(szLogInfo);
				exit(0);
			}
		}
	}
	return id;
}