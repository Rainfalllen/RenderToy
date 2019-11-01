#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "../Shader.h"
#include "../Camera.h"

#include <string>
using std::string;

class Light
{
public:
	Light(string _lightName, glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		position = _position;
		angles = _angles;
		color = _color;
		UpdateDirection();

		LightNameInShader = _lightName;

		ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		specular = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	Light() {}
	~Light() {}


	glm::vec3 position;
	glm::vec3 angles;
	glm::vec3 direction = glm::vec3(0, 0, 1.0f);
	glm::vec3 color;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;


	void UpdateDirection()
	{
		direction = glm::vec3(0, 0, 1.0f);
		direction = glm::rotateZ(direction, angles.z);
		direction = glm::rotateX(direction, angles.x);
		direction = glm::rotateY(direction, angles.y);
		direction = -direction;
	}

	virtual void SetLightInfoInShader(Shader* shader, Camera* camera)
	{
		shader->use();

		shader->SetUniformVec3(LightNameInShader + ".position", position);

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

			shader[i].SetUniformVec3(LightNameInShader + ".position", position);

			shader[i].SetUniformVec3(LightNameInShader + ".ambient", ambient);
			shader[i].SetUniformVec3(LightNameInShader + ".diffuse", diffuse);
			shader[i].SetUniformVec3(LightNameInShader + ".specular", specular);
			
			shader[i].SetUniformVec3("viewPos", camera->GetPosition());
		}
	}

protected:
	string LightNameInShader;
};

#endif // !LIGHT_H

