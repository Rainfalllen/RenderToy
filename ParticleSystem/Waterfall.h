#ifndef WATERFALL_H
#define WATERFALL_H

#include"ParticleSystem.h"

struct WaterfallParticle 
{
	glm::vec3 position;
	glm::vec3 speed;
	GLuint lifespan;
	bool bounced;
};

class Waterfall :public ParticleSystem<WaterfallParticle>
{
public:
	Waterfall();
	virtual void Update(GLfloat deltaTime);
	virtual void Render();
	void TestRender();
private:
	virtual void RenderParticle(const WaterfallParticle& p);
};

#endif


#include "../Primitive/Cube.h"
#include "../Primitive/Sphere.h"
#include <random>

Shader* w_shader;
//Cube* cube;

static const float gravity = 9.8f;

const float pi = 3.1416;
float randFloat01() {
	return 1.0 * rand() / RAND_MAX;
}
float randFloat(float from, float to) {
	return from + (to - from)*randFloat01();
}
int randInt(int from, int to) {
	return from + rand() % (to - from);
}

Waterfall::Waterfall()
{
}


void Waterfall::Update(GLfloat deltaTime)
{
	// new particles
	WaterfallParticle particle;
	int newParticleNumber = randInt(2, 4);
	for (int i = 0; i < newParticleNumber; ++i) {
		particle.position = glm::vec3(-2.0f, 1.8f, 0.0f);
		particle.speed = glm::vec3(randFloat(3.0f, 5.0f), randFloat(-1.0f, 1.0f), randFloat(-1.0f, 1.0f));
		particle.lifespan = mParticleLifespan;
		particle.bounced = false;
		CreateParticle(particle);
	}

	// existing particles
	for (int i = 0; i < mParticleNumber; ++i) 
	{
		if (mParticlePool[i].link.mark != 1) 
		{
			WaterfallParticle* p = &(mParticlePool[i].particle);//particle表示当前循环要更新的粒子
			p->position += p->speed * deltaTime;
			if (p->position.x > -2.0f) {
				p->speed.y -= gravity * deltaTime;
			}
			if (p->position.y < 0 && !p->bounced) {
				p->bounced = true;
				p->speed.y *= -randFloat(0.4f, 0.7f);
				p->speed.x *= randFloat(0.6f, 0.9f);
				p->speed.z *= randFloat(0.6f, 0.9f);
			}
			if (p->lifespan <= 0 || p->position.y < -10.0f) {
				DestroyParticle(i);
			}
			p->lifespan--;
		}
	}
}

void Waterfall::Render()
{
	for (GLint i = 0; i < mParticleNumber; ++i) {
		if (mParticlePool[i].link.mark != 1) {
			RenderParticle(mParticlePool[i].particle);
		}
	}
}

//void Waterfall::TestRender()
//{
//	WaterfallParticle particle;
//	int newParticleNumber = randInt(2, 4);
//	for (int i = 0; i < newParticleNumber; ++i) 
//	{
//		particle.position = glm::vec3(0.0f, 0.0f, 0.0f);
//		particle.speed = glm::vec3(randFloat(9.0f, 12.0f), randFloat(-1.0f, 1.0f), randFloat(-1.0f, 1.0f));
//		particle.lifespan = mParticleLifespan;
//		particle.bounced = false;
//		RenderParticle(particle);
//	}
//}

void Waterfall::RenderParticle(const WaterfallParticle & particle)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, particle.position);
	model = glm::rotate(model, randFloat(0.0f, 180), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, randFloat(0.0f, 180), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));

	w_shader->use();
	w_shader->SetUniformMat4("model", model);
	w_shader->SetUniformVec3("color", glm::vec3(randFloat(0.1f, 0.3f), randFloat(0.2f, 0.4f), randFloat(0.8f, 1.0f)));

	//Cube::Draw();
	Sphere::Draw();
}
