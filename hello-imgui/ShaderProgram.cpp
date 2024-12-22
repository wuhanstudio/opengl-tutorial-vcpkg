#include "ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <fmt/core.h>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram()
	: mHandle(0)
{
    
}

ShaderProgram::~ShaderProgram()
{
	// Don't do this
	// glDeleteProgram(mHandle);
}

void ShaderProgram::destroy()
{
	if (mHandle != 0) {
		glDeleteProgram(mHandle);
	}
}

//-----------------------------------------------------------------------------
// Loads vertex and fragment shaders
//-----------------------------------------------------------------------------
bool ShaderProgram::loadShaders(const char* vsFilename, const char* fsFilename)
{
	string vsString = fileToString(vsFilename);
	string fsString = fileToString(fsFilename);

	const GLchar* vsSourcePtr = vsString.c_str();
	const GLchar* fsSourcePtr = fsString.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);

	glCompileShader(vs);
	checkCompileErrors(vs, VERTEX);

	glCompileShader(fs);
	checkCompileErrors(fs, FRAGMENT);

	mHandle = glCreateProgram();
	if (mHandle == 0)
	{
		fmt::println("Unable to create shader program!");
		return false;
	}

	glAttachShader(mHandle, vs);
	glAttachShader(mHandle, fs);

	glLinkProgram(mHandle);
	checkCompileErrors(mHandle, PROGRAM);

	glDeleteShader(vs);
	glDeleteShader(fs);

	mUniformLocations.clear();

	return true;
}

void ShaderProgram::use()
{
	if (mHandle > 0)
		glUseProgram(mHandle);
}

//-----------------------------------------------------------------------------
// Opens and reads contents of ASCII file to a string.  Returns the string.
// Not good for very large files.
//-----------------------------------------------------------------------------
string ShaderProgram::fileToString(const string& filename)
{
	std::stringstream ss;
	std::ifstream file;

	try
	{
		file.open(filename, std::ios::in);

		if (!file.fail())
		{
			// Using a std::stringstream is easier than looping through each line of the file
			ss << file.rdbuf();
		}

		file.close();
	}
	catch (std::exception ex)
	{
		fmt::println("Error reading shader filename!");
	}

	return ss.str();
}

//-----------------------------------------------------------------------------
// Returns the active shader program
//-----------------------------------------------------------------------------
const GLuint ShaderProgram::getProgram()
{
	return mHandle;
}

//-----------------------------------------------------------------------------
// Checks for shader compiler errors
//-----------------------------------------------------------------------------
void  ShaderProgram::checkCompileErrors(GLuint shader, ShaderType type)
{
	int status = 0;

	if (type == PROGRAM)
	{
		glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint length = 0;
			glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);

			// The length includes the NULL character
			string errorLog(length, ' ');	// Resize and fill with space character
			glGetProgramInfoLog(mHandle, length, &length, &errorLog[0]);

			fmt::println("Error!Shader failed to compile. {}", errorLog);
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			// The length includes the NULL character
			string errorLog(length, ' ');  // Resize and fill with space character
			glGetShaderInfoLog(shader, length, &length, &errorLog[0]);

			fmt::println("Error!Shader failed to compile. {}", errorLog);
		}
	}

}

//-----------------------------------------------------------------------------
// Sets a glm::vec2 shader uniform
//-----------------------------------------------------------------------------
void ShaderProgram::setUniform(const GLchar* name, const glm::vec2& v)
{
	GLint loc = getUniformLocation(name);
	glUniform2f(loc, v.x, v.y);
}

//-----------------------------------------------------------------------------
// Sets a glm::vec3 shader uniform
//-----------------------------------------------------------------------------
void ShaderProgram::setUniform(const GLchar* name, const glm::vec3& v)
{
	GLint loc = getUniformLocation(name);
	glUniform3f(loc, v.x, v.y, v.z);
}

//-----------------------------------------------------------------------------
// Sets a glm::vec4 shader uniform
//-----------------------------------------------------------------------------
void ShaderProgram::setUniform(const GLchar* name, const glm::vec4& v)
{
	GLint loc = getUniformLocation(name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

//-----------------------------------------------------------------------------
// Sets a glm::mat4 shader uniform
//-----------------------------------------------------------------------------
void ShaderProgram::setUniform(const GLchar* name, const glm::mat4& m)
{
	GLint loc = getUniformLocation(name);

	// loc = location of uniform in shader
	// count = how many matrices (1 if not an array of mats)
	// transpose = False for opengl because column major
	// value = the matrix to set for the uniform
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

//-----------------------------------------------------------------------------
// Sets a GLfloat shader uniform
//-----------------------------------------------------------------------------
void ShaderProgram::setUniform(const GLchar* name, const GLfloat f)
{
	GLint loc = getUniformLocation(name);
	glUniform1f(loc, f);
}

//-----------------------------------------------------------------------------
// Sets a GLint shader uniform
//-----------------------------------------------------------------------------
void ShaderProgram::setUniform(const GLchar* name, const GLint v)
{
	GLint loc = getUniformLocation(name);
	glUniform1i(loc, v);
}

//-----------------------------------------------------------------------------
// Sets a GLint shader uniform that is specific to a texture unit
//-----------------------------------------------------------------------------
void ShaderProgram::setUniformSampler(const GLchar* name, const GLint& slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);

	GLint loc = getUniformLocation(name);
	glUniform1i(loc, slot);
}

//-----------------------------------------------------------------------------
// Returns the uniform identifier given it's string name.
// NOTE: Shader must be currently active first.
//-----------------------------------------------------------------------------
GLint ShaderProgram::getUniformLocation(const GLchar* name)
{
	std::map<string, GLint>::iterator it = mUniformLocations.find(name);

	// Only need to query the shader program IF it doesn't already exist.
	if (it == mUniformLocations.end())
	{
		// Find it and add it to the map
		mUniformLocations[name] = glGetUniformLocation(mHandle, name);
	}

	// Return it
	return mUniformLocations[name];
}
