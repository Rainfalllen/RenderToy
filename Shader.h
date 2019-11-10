#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

using std::ifstream;
using std::stringstream;
using std::string;

class Shader
{
public:
	string vertexString;
	string fragmentString;
	const char* vertexSource;
	const char* fragmentSource;

	unsigned int ID;

	Shader(const char * vertexPath, const char * fragmentPath, const char * geometryPath = nullptr)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		string vertexCode;
		string fragmentCode;
		string geometryCode;
		ifstream vertexFile;
		ifstream fragmentFile;
		ifstream geometryFile;

		// ensure ifstream objects can throw exceptions:

		vertexFile.exceptions(ifstream::failbit || ifstream::badbit);
		fragmentFile.exceptions(ifstream::failbit || ifstream::badbit);
		geometryFile.exceptions(ifstream::failbit || ifstream::badbit);

		try
		{
			// open files
			vertexFile.open(vertexPath);
			fragmentFile.open(fragmentPath);
			stringstream vertexStream;
			stringstream fragmentStream;

			// read file's buffer contents into streams
			vertexStream << vertexFile.rdbuf();
			fragmentStream << fragmentFile.rdbuf();

			// close file handlers
			vertexFile.close();
			fragmentFile.close();

			// convert stream into string
			vertexCode = vertexStream.str();
			fragmentCode = fragmentStream.str();


			// if geometry w_shader path is present, also load a geometry w_shader
			if (geometryPath != nullptr)
			{
				geometryFile.open(geometryPath);
				std::stringstream geometryStream;
				geometryStream << geometryFile.rdbuf();
				geometryFile.close();
				geometryCode = geometryStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR! Shader file can't be read successfully" << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. compile shaders
		unsigned int vertex, fragment;
		
		// vertex w_shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		
		// if geometry w_shader is given, compile geometry w_shader
		unsigned int geometry;
		if (geometryPath != nullptr)
		{
			const char * gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}
		
		// w_shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);
	}
	Shader() {}
	~Shader(){}
	
	Shader &operator=(Shader s)
	{
		vertexString = s.vertexString;
		fragmentString = s.fragmentString;
		vertexSource = s.vertexSource;
		fragmentSource = s.fragmentSource;

		ID = s.ID;
		return *this;
	}

	void use()
	{
		glUseProgram(ID);
	}

	void SetUniform3f(const char * paramNameString, glm::vec3 param)
	{
		int location = glGetUniformLocation(ID, paramNameString);
		if (location < 0)
		{
			printf("It didn't get %s in shader file", paramNameString);
			return;
		}
		glUniform3f(location, param.x, param.y, param.z);
	}

	void SetUniform1f(const char * paramNameString, float param)
	{
		glUniform1f(glGetUniformLocation(ID, paramNameString), param);
	}

	void SetUniform1f(const std::string paramNameString, float param)
	{
		glUniform1f(glGetUniformLocation(ID, paramNameString.c_str()), param);
	}

	void SetUniform1i(const char * paramNameString, int slot)
	{
		glUniform1i(glGetUniformLocation(ID, paramNameString), slot);
	}

	void SetUniform1i(const std::string paramNameString, int slot)
	{
		glUniform1i(glGetUniformLocation(ID, paramNameString.c_str()), slot);
	}

	void SetUniformVec4(const std::string & name, const glm::vec4 & value)
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void SetUniformMat4(const std::string & name, const glm::mat4 & mat)
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void SetUniformVec3(const std::string & name, const glm::vec3 & value)
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void SetUniformsVec3(const std::string & name, int n, const glm::vec3 value[])
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), n, &value[0][0]);
	}


private:
	void checkCompileErrors(unsigned int ID, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(ID, 1024, NULL, infoLog);
				std::cout << "ERROR! Shader compilation error: " << type << "\n" << infoLog << "\n ----------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(ID, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(ID, 1024, NULL, infoLog);
				std::cout << "ERROR! Program linking error: " << type << "\n" << infoLog << "\n ----------------------------------------------------- -- " << std::endl;
			}
		}
	}
};


#endif
