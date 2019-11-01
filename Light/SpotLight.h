#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include"Light.h"

class SpotLight: public Light
{
public:
	SpotLight(string _lightName, glm::vec3 _position, glm::vec3 _angles,
		glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f)) : Light(_lightName, _position, _angles, _color)
	{
		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;

		cosPhyInner = 0.9f;
		cosPhyOutter = 0.85f;

		ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		specular = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	float constant;
	float linear;
	float quadratic;

	float cosPhyInner;
	float cosPhyOutter;

	virtual void SetLightInfoInShader(Shader* shader, Camera* camera)
	{
		shader->use();

		shader->SetUniformVec3(LightNameInShader + ".position", position);
		shader->SetUniformVec3(LightNameInShader + ".ambient", ambient);
		shader->SetUniformVec3(LightNameInShader + ".diffuse", diffuse);
		shader->SetUniformVec3(LightNameInShader + ".specular", specular);
		shader->SetUniform1f(LightNameInShader + ".constant", constant);
		shader->SetUniform1f(LightNameInShader + ".linear", linear);
		shader->SetUniform1f(LightNameInShader + ".quadratic", quadratic);
		shader->SetUniform1f(LightNameInShader + ".cosPhyInner", cosPhyInner);
		shader->SetUniform1f(LightNameInShader + ".cosPhyOutter", cosPhyOutter);

		shader->SetUniformVec3("viewPos", camera->GetPosition());
		shader->SetUniformVec3(LightNameInShader + ".direction", glm::vec3(0, 0, 1.0));
	}

	virtual void SetLightInfoInShaders(Shader* shader, int n, Camera* camera)
	{
		for (int i = 0; i < n; i++)
		{
			shader[i].use();

			shader[i].SetUniformVec3(LightNameInShader + ".position", position);
			shader[i].SetUniformVec3(LightNameInShader + ".ambient", ambient);
			shader[i].SetUniformVec3(LightNameInShader + ".diffuse", diffuse);
			shader[i].SetUniformVec3(LightNameInShader + ".specular", specular);
			shader[i].SetUniform1f(LightNameInShader + ".constant", constant);
			shader[i].SetUniform1f(LightNameInShader + ".linear", linear);
			shader[i].SetUniform1f(LightNameInShader + ".quadratic", quadratic);
			shader[i].SetUniform1f(LightNameInShader + ".cosPhyInner", cosPhyInner);
			shader[i].SetUniform1f(LightNameInShader + ".cosPhyOutter", cosPhyOutter);

			shader[i].SetUniformVec3("viewPos", camera->GetPosition());
		}
	}


};

#endif

