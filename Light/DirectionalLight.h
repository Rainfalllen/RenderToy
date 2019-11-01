#ifndef LIGHTDIRECTIONAL_H
#define LIGHTDIRECTIONAL_H

#include "Light.h"

class DirectionalLight: public Light
{
public:
	DirectionalLight(string _lightName, glm::vec3 _direction,
		glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		position = glm::vec3(0, 0, 0);
		direction = _direction;
		angles = glm::vec3(0, 0, 0);
		color = _color;
		
		LightNameInShader = _lightName;

		ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		specular = glm::vec3(0.5f, 0.5f, 0.5f);
	}

	virtual void SetLightInfoInShader(Shader* shader, Camera* camera)
	{
		shader->use();

		shader->SetUniformVec3(LightNameInShader + ".direction", direction);
		shader->SetUniformVec3(LightNameInShader + ".ambient", ambient);
		shader->SetUniformVec3(LightNameInShader + ".diffuse", diffuse);
		shader->SetUniformVec3(LightNameInShader + ".specular", specular);

		shader->SetUniformVec3("viewPos", camera->GetPosition());
	}

	virtual void SetLightInfoInShaders(Shader* shader, int n, Camera* camera)
	{
		for (int i = 0; i < n; i++)
		{
			shader[i].use();
				   
			shader[i].SetUniformVec3(LightNameInShader + ".direction", direction);
			shader[i].SetUniformVec3(LightNameInShader + ".ambient", ambient);
			shader[i].SetUniformVec3(LightNameInShader + ".diffuse", diffuse);
			shader[i].SetUniformVec3(LightNameInShader + ".specular", specular);
				   
			shader[i].SetUniformVec3("viewPos", camera->GetPosition());
		}
	}
};


#endif 

