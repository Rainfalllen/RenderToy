#include<GL/glew.h>
#include<GL/freeglut.h>
#include<time.h>

#include <iostream>
#include <vector>
#include <string>
#include "Model.h"
#include "Camera.h"
#include "ParticleSystem/Waterfall.h"
#include "GLWindow.h"

using namespace std;


const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
//Camera camera(glm::vec3(10.0f, 0.0f, 0.0f), 0,0, glm::vec3(0, 1.0f, 0));
float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
bool firstMouse = true;

Waterfall* waterfall;

GLWindow* glWindow;

float deltaTime = 0.0;
float lastFrame = 0.0;
bool first = true;

void display()
{
	// delta time logic
	float currentFrame = clock();
	deltaTime = (currentFrame - lastFrame) / CLOCKS_PER_SEC;
	lastFrame = currentFrame;

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//-----------------------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();

	w_shader->use();
	w_shader->SetUniformMat4("projection", projection);
	w_shader->SetUniformMat4("view", view);

	waterfall->Update(deltaTime);
	waterfall->Render();
	
	//-----------------------------------------------------------------
	glutPostRedisplay();
	glutSwapBuffers();

	camera.UpdateCameraPosition();
}



void reshape(int width, int height);
void keyboard(unsigned char key, int xpos, int ypos);
void mousebutton(int button, int state, int x, int y);
void mousemovement(int xpos, int ypos);


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glWindow = new GLWindow(WIDTH, HEIGHT);
	// -----------------------------------------------------------
	w_shader = new Shader("ParticleSystem/waterfall.vs", "ParticleSystem/waterfall.fs");
	waterfall = new Waterfall();
	// -----------------------------------------------------------
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemovement);

	glutMainLoop();

	delete glWindow;
	return 0;
}

// -----------------------------------------------------------------
void reshape(int width, int height)
{
	//glViewport(0, 0, width, height);
}

void keyboard(unsigned char key, int xpos, int ypos)
{
	switch (key) {
	case 033: // Escape Key
		exit(EXIT_SUCCESS);
		break;

	case 'w':
	case 'W':
		camera.speedZ = 2.0f;
		break;
	case 's':
	case 'S':
		camera.speedZ = -2.0f;
		break;
	case 'd':
	case 'D':
		camera.speedX = 2.0f;
		break;
	case 'a':
	case 'A':
		camera.speedX = -2.0f;
		break;
	case 'q':
	case 'Q':
		camera.speedY = -2.0f;
		break;
	case 'e':
	case 'E':
		camera.speedY = 2.0f;
		break;
	}

	glutPostRedisplay();
}

void mousebutton(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		firstMouse = true;
	}
}

void mousemovement(int xpos, int ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
		return;
	}

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset / 5, yoffset / 5, true);

}


