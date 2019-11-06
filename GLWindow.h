#ifndef GLWINDOW_H
#define GLWINDOW_H

#include<GL/glew.h>
#include<GL/freeglut.h>
#include <iostream>

#include "Camera.h"


class GLWindow
{
public:
	typedef void* (*FUNC)(void*);

	GLWindow(GLuint width,GLuint height)
	{
		WIDTH = width;
		HEIGHT = height;
		Init();

		glewExperimental = true;
		if (glewInit() != GLEW_OK)
		{
			std::cout << "Init GLEW failed\n";
		}
	}

	~GLWindow() 
	{
		glutDestroyWindow(window);
	}

	void SetRenderScene(void(*callback)())
	{
		glutDisplayFunc(callback);
	}

	void Reshape(void(*callback)(int,int))
	{
		glutReshapeFunc(callback);
	}

private:
	GLint window;
	GLuint WIDTH;
	GLuint HEIGHT;

	Camera* camera;
	float lastX = (float)WIDTH / 2.0;
	float lastY = (float)HEIGHT / 2.0;
	bool firstMouse = true;

	void Init()
	{
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(WIDTH, HEIGHT);
		window = glutCreateWindow("Emmm");
		glEnable(GL_DEPTH_TEST);

		// Hide the cursor
		glutSetCursor(GLUT_CURSOR_NONE);
	}


};

#endif
