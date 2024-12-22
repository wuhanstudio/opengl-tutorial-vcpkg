#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "ShaderProgram.h"

class Skybox
{
public:
	Skybox(const std::vector<std::string>& faces);
	~Skybox();

	GLuint loadCubemap(const std::vector<std::string>& faces);
	void render(ShaderProgram skyboxShader, const glm::mat4& view, const glm::mat4& projection);

	void destroy();

private:
	GLuint mSkyboxVertices;
	GLuint mVAO, mVBO;
	GLuint mCubemapTexture;
};

#endif // SKYBOX_H
