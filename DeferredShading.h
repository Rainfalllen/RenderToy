#ifndef DEFERREDSHADING_H
#define DEFERREDSHADING_H

#include "Shader.h"
#include "Texture.h"

class DeferredShading
{
public:
	DeferredShading(uint _WIDTH, uint _HEIGHT)
	{
		SCR_WIDTH = _WIDTH, SCR_HEIGHT = _HEIGHT;
		Init();
	}

	void UseTexture()
	{
		gPosition->Use(0);
		gNormal->Use(1);
		gAlbedoSpec->Use(2);
	}

	GLuint gBuffer;
private:
	GLuint rboDepth;
	Texture* gPosition;
	Texture* gNormal;
	Texture* gAlbedoSpec;
	uint SCR_WIDTH, SCR_HEIGHT;


	void Init()
	{
		// Set up G-Buffer
		// 3 textures:
		// 1. Positions (RGB)
		// 2. Color (RGB) + Specular (A)
		// 3. Normals (RGB) 
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	
		// - Position color buffer
		gPosition = new Texture(SCR_WIDTH, SCR_HEIGHT, nullptr, GL_FLOAT, GL_RGB, GL_RGB16F);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition->GetID(), 0);
		// - Normal color buffer
		gNormal = new Texture(SCR_WIDTH, SCR_HEIGHT, nullptr, GL_FLOAT, GL_RGB, GL_RGB16F);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal->GetID(), 0);
		// - Color + Specular color buffer
		gAlbedoSpec = new Texture(SCR_WIDTH, SCR_HEIGHT, nullptr, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec->GetID(), 0);
		
		// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		// - Create and attach depth buffer (renderbuffer)
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		
		// - Finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

};

#endif
