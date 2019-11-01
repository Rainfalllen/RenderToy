#include <iostream>
#include <vector>
#include <string>
#include "Model.h"
#include "Camera.h"

#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"

#include "Skybox.h"
#include "PostProcessing.h"
#include "GLWindow.h"

#include "Shadow.h"

using namespace std;

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
bool firstMouse = true;

GLWindow* glWindow;
Model* _floor;
Shader* gamma_correction_shader;



// lighting info
// -------------
glm::vec3 lightPositions[] = {
	glm::vec3(-3.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(3.0f, 0.0f, 0.0f)
};
glm::vec3 lightColors[] = {
	glm::vec3(0.25),
	glm::vec3(0.50),
	glm::vec3(0.75),
	glm::vec3(1.00)
};

float planeVertices[] = {
	// positions            // normals         // texcoords
	 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
	-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

	 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
	-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
	 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};


float angle = 0;
void display()
{
	//postProcess->Begin();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 使用OpenGL内建的sRGB帧缓冲
	//
	glEnable(GL_FRAMEBUFFER_SRGB);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	
	gamma_correction_shader->use();
	gamma_correction_shader->SetUniformMat4("projection", projection);
	gamma_correction_shader->SetUniformMat4("view", view);
	gamma_correction_shader->SetUniformMat4("model", glm::mat4(1.0));

	gamma_correction_shader->SetUniformVec3("viewPos", camera.GetPosition());
	gamma_correction_shader->SetUniform1i("gamma", 0);

	gamma_correction_shader->SetUniformsVec3("lightPositions", 4, lightPositions);
	gamma_correction_shader->SetUniformsVec3("lightColors", 4, lightColors);
	_floor->Draw();
	

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

	gamma_correction_shader = new Shader("shader_file/gamma_correction.vs","shader_file/gamma_correction.fs");

	_floor = new Model(planeVertices, sizeof(planeVertices)/sizeof(planeVertices[0]), 
		gamma_correction_shader, "Image/wood.png");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemovement);

	glutMainLoop();

	delete glWindow;

	return 0;
}

// Some callback functions
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
		camera.speedX = 1.0f;
		break;
	case 'a':
	case 'A':
		camera.speedX = -1.0f;
		break;
	case 'q':
	case 'Q':
		camera.speedY = -1.0f;
		break;
	case 'e':
	case 'E':
		camera.speedY = 1.0f;
		break;


	case 'b':
	case 'B':
		
		break;
		//case 'r':
		//case 'R':
		//	glm::mat4 view = camera.GetViewMatrixLookAtTarget();
		//	shader->use();
		//	shader->SetUniformMat4("view", view);
		//	break;
	}

	glutPostRedisplay();
}

void mousebutton(int button, int state, int x, int y)
{
	if (state == GLUT_UP)
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