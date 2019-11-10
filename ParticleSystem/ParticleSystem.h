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
	// ���ö����ģʽ
	union PoolAllocUnit 
	{
		ParticleType particle;
		struct Link 
		{
			GLint mark;//�ж��Ƿ�Ϊlink�ı�־����Ϊ1
			GLint next;//freelist��ʹ�õ�������
		}link;
		PoolAllocUnit() {}
	};
	//�ط������ĵ�ַ
	PoolAllocUnit* mParticlePool;

	GLuint mParticleNumber;
	GLuint mParticleLifespan;
private:
	//��ʾһ����freelist�е����������������freelistջ��ջ��Ԫ�أ�
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
	mParticlePool[mParticleNumber - 1].link.next = -1;//-1��ǵ�ǰfreelistֻʣ�����һ��Ԫ��
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
	if (index == -1)return;//�����ǰ���������ѳ����趨�������������������ֱ�ӷ���
	mParticlePool[mFreeIndex].particle = particle;
	mFreeIndex = index;
}

template<typename ParticleType>
void ParticleSystem<ParticleType>::DestroyParticle(GLint index)
{
	if (index < 0 || index >= mParticleNumber)	return;
	if (mParticlePool[index].link.mark == 1)	return; //index����freelist
	mParticlePool[index].link.mark = 1;//��index��ӵ�freelist
	mParticlePool[index].link.next = mFreeIndex;
	mFreeIndex = index;
}

#endif
