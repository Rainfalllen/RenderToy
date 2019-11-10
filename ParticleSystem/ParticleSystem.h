#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#define DEFAULT_PARTICLE_NUMBER 10000
#define DEFAULT_PARTICLE_LIFESPAN 1000

template<typename ParticleType>
class ParticleSystem 
{
public:
	ParticleSystem(GLuint particleNumber = DEFAULT_PARTICLE_NUMBER,
		GLuint particleLifespan = DEFAULT_PARTICLE_LIFESPAN);

	virtual ~ParticleSystem();

	virtual void Render();
	virtual void Update(GLfloat deltaTime) = 0;

	bool inUse() const { return mParticleLifespan > 0; }

protected:
	virtual void RenderParticle(const ParticleType& p) = 0;

	void CreateParticle(const ParticleType& p);
	void DestroyParticle(GLint index);

protected:
	// 采用对象池模式
	union PoolAllocUnit 
	{
		ParticleType particle;
		struct Link 
		{
			GLint mark;//判断是否为link的标志，设为1
			GLint next;//freelist，使用单向链表
		}link;
		PoolAllocUnit() {}
	};
	//池分配器的地址
	PoolAllocUnit* mParticlePool;

	GLuint mParticleNumber;
	GLuint mParticleLifespan;
private:
	//表示一个在freelist中的粒子数组的索引（freelist栈的栈顶元素）
	GLint mFreeIndex;
};

template<typename ParticleType>
inline ParticleSystem<ParticleType>::ParticleSystem(GLuint particleNumber, GLuint particleLifespan)
	:mParticleNumber(particleNumber), mParticleLifespan(particleLifespan)
{
	// dynamic memory allocation according to the number of particles
	mParticlePool = new PoolAllocUnit[mParticleNumber];
	// initiate freelist
	memset(mParticlePool, 0, sizeof(PoolAllocUnit) * mParticleNumber);
	mFreeIndex = 0;
	for (GLint i = 0; i < mParticleNumber; ++i)
	{
		mParticlePool[i].link.mark = 1;
		mParticlePool[i].link.next = i + 1;
	}
	mParticlePool[mParticleNumber - 1].link.next = -1;//-1标记当前freelist只剩最后这一个元素
}

template<typename ParticleType>
inline ParticleSystem<ParticleType>::~ParticleSystem()
{
	delete[] mParticlePool;
}

template<typename ParticleType>
void ParticleSystem<ParticleType>::Render()
{
	for (GLint i = 0; i < mParticleNumber; ++i) {
		if (mParticlePool[i].link.mark != 1) {
			RenderParticle(mParticlePool[i].particle);
		}
	}
}

template<typename ParticleType>
void ParticleSystem<ParticleType>::CreateParticle(const ParticleType& particle)
{
	GLint index = mParticlePool[mFreeIndex].link.next;
	if (index == -1)return;//如果当前粒子数量已超过设定的最大粒子数量，则函数直接返回
	mParticlePool[mFreeIndex].particle = particle;
	mFreeIndex = index;
}

template<typename ParticleType>
void ParticleSystem<ParticleType>::DestroyParticle(GLint index)
{
	if (index < 0 || index >= mParticleNumber)	return;
	if (mParticlePool[index].link.mark == 1)	return; //index已在freelist
	mParticlePool[index].link.mark = 1;//将index添加到freelist
	mParticlePool[index].link.next = mFreeIndex;
	mFreeIndex = index;
}

#endif
