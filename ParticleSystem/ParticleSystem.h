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

protected:
	virtual void RenderParticle(const ParticleType& p) = 0;

	void CreateParticle(const ParticleType& p);
	void DestroyParticle(GLint index);

protected:
	//池分配器的分配单元，可以用来保存粒子信息或freelist信息
	union PoolAllocUnit 
	{
		ParticleType particle;
		struct Link 
		{
			GLint mark;//判断是否为link的标志，设为1
			GLint next;//使用“栈”的数据结构存储freelist，所以使用单向链表即可
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
	//dynamic memory allocation according to the number of particles
	mParticlePool = new PoolAllocUnit[mParticleNumber];
	//初始化freelist
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
	//渲染每一个“存在”的粒子
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
	if (index < 0 || index >= mParticleNumber)return;//索引不合法
	if (mParticlePool[index].link.mark == 1)return;//当前索引在freelist中
	mParticlePool[index].link.mark = 1;//当前索引添加到freelist
	mParticlePool[index].link.next = mFreeIndex;
	mFreeIndex = index;
}

#endif
