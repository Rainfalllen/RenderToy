#include<GL/glew.h>
#include<GL/freeglut.h>

#include <iostream>
#include <vector>
#include <string>
#include "Model.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class GLWindow
{
	void Init() 
	{
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(500, 500);
		glutCreateWindow("haha");
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_SMOOTH);
		SetLight();
		glEnable(GL_DEPTH_TEST);
	}


	// load and create a texture 
	// -------------------------
	GLuint texture;
	void SetTexture(const char*filename)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// load image, create texture and generate mipmaps
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
		unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	//安置光源
	void SetLight() 
	{
		GLfloat lightPosition[] = { 0.0f, 0.0f, 1.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glEnable(GL_LIGHTING); //启用光源
		glEnable(GL_LIGHT0);   //使用指定灯光
	}
};