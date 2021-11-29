#ifndef SHADER_H
#define SHADER_H

#include<string>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader(const std::string& filepath);
	unsigned int CreateProgram(const std::string& vertexSource, const std::string& fragmentSource);
	void Bind();
	void Unbind();
	int GetUniformLocation(const std::string& name);
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetVec3(const std::string& name, const glm::vec3 &v);
	void SetUniformMatrix4fv(const std::string& name, glm::mat4& value);

	std::string m_vertexShaderSource;
	std::string m_fragmentShaderSource;
	uint8_t m_programID;

private:
	unsigned int CompileShader(unsigned int type, const std::string& shaderSource);
};
#endif

