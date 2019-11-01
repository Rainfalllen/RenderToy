#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include "Shader.h"

float screenVertices[] =
{
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

class PostProcessing
{
public:
	PostProcessing(int width, int height, Shader* shader)
	{
		SetupScreen(width, height, shader);
	}

	~PostProcessing() 
	{
		glDeleteVertexArrays(1, &screenVAO);
		glDeleteBuffers(1, &screenVBO);
		glDeleteFramebuffers(1, &framebuffer);
	}


	void Draw(Shader* shader)
	{
		shader->use();
		glBindVertexArray(screenVAO);
		glBindTexture(GL_TEXTURE_2D, frameToTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void Begin()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);

		// make sure we clear the framebuffer's content
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void End(Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		Draw(shader);
	}

private:
	GLuint screenVAO, screenVBO;
	GLuint framebuffer;
	GLuint frameToTexture;
	GLuint RBO;
	Shader* shader;

	void SetupScreen(int width,int height, Shader* shader)
	{
		glGenVertexArrays(1, &screenVAO);
		glGenBuffers(1, &screenVBO);
		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		shader->use();
		shader->SetUniform1i("screenTexture", 0);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &frameToTexture);
		glBindTexture(GL_TEXTURE_2D, frameToTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameToTexture, 0);
	
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it
		
																									  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			printf("Error: Framebuffer is not complete!\n");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// mark掉下面这行程式码的渲染效果和深度测试中是完全一样的，但这次是渲染在一个简单的四边形上。
		// 如果我们使用线框模式渲染场景，就会变得很明显
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
};


#endif

