#ifndef SKYBOX_H
#define SKYBOX_H

#include "Shader.h"
#include "Texture.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

const float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

class Skybox
{
public:
	Skybox(const vector<string> path)
	{
		faces = path;
		Init();
	}

	Skybox()
	{
		faces = {
		"Resource/Image/skybox/right.jpg",
		"Resource/Image/skybox/left.jpg",
		"Resource/Image/skybox/top.jpg",
		"Resource/Image/skybox/bottom.jpg",
		"Resource/Image/skybox/front.jpg",
		"Resource/Image/skybox/back.jpg" };

		Init();
	}

	~Skybox()
	{
		glDeleteVertexArrays(1, &skyboxVAO);
		glDeleteBuffers(1, &skyboxVBO);
		delete skyboxTex;
	}

	void Draw()
	{
		skybox_shader->use();
		glDepthFunc(GL_LEQUAL);
		glBindVertexArray(skyboxVAO);

		skyboxTex->Use(0);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}

	void SetShaderInfo(glm::mat4 projection, glm::mat4 view)
	{
		skybox_shader->use();
		skybox_shader->SetUniformMat4("projection", projection);
		skybox_shader->SetUniformMat4("view", view);
	}

	void ChangeShader(Shader* newShader)
	{
		skybox_shader = newShader;
		printf("The skybox shader has been changed.\n");
	}

private:
	void Init()
	{
		skybox_shader = new Shader("shader_file/Skybox/skybox.vs", "shader_file/Skybox/skybox.fs");

		glGenVertexArrays(1, &skyboxVAO);
		glBindVertexArray(skyboxVAO);

		glGenBuffers(1, &skyboxVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		skyboxTex = new Texture(faces);
		skybox_shader->use();
		skybox_shader->SetUniform1i("skyboxTex", 0);
	}


	vector<string> faces;
	GLuint skyboxVAO, skyboxVBO;
	GLuint skyboxTexture;
	Texture* skyboxTex;
	Shader* skybox_shader;
};

#endif // !SKYBOX_H

