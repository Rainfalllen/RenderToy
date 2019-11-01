#include<GL/glew.h>
#include<GL/freeglut.h>

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
#include "DeferredShading.h"

using namespace std;


const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
//Camera camera(glm::vec3(10.0f, 0.0f, 0.0f), 0,0, glm::vec3(0, 1.0f, 0));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

Shader* shaderGeometryPass;
Shader* shaderLightingPass;

Shader* test;
GLint window;
DeferredShading* deferred;
Model* cyborg;
Model* _floor;

vector<glm::vec3> objectPositions;
const GLuint NR_LIGHTS = 32;
vector<glm::vec3> lightPositions;
vector<glm::vec3> lightColors;

GLuint gBuffer;
GLuint gPosition, gNormal, gAlbedoSpec;
GLuint rboDepth;

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


float angle = 0;
void display()
{
	// 1. Geometry Pass: render scene's geometry/color data into gbuffer
	//glBindFramebuffer(GL_FRAMEBUFFER, deferred->gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model;

	//shaderGeometryPass->use();
	//shaderGeometryPass->SetUniformMat4("projection", projection);
	//shaderGeometryPass->SetUniformMat4("view", view);
	//for (GLuint i = 0; i < objectPositions.size(); i++)
	//{
	//	model = glm::mat4();
	//	model = glm::translate(model, objectPositions[i]);
	//	//model = glm::scale(model, glm::vec3(0.25f));
	//	shaderGeometryPass->SetUniformMat4("model", model);
	//	cyborg->Draw(shaderGeometryPass, true);
	//}

	test->use();
	test->SetUniformMat4("projection", projection);
	test->SetUniformMat4("view", view);
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0,0,0));
	test->SetUniformMat4("model", model);
	_floor->Draw();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//shaderLightingPass->use();
	//deferred->UseTexture();
	//
	//// Also send light relevant uniforms
	//for (GLuint i = 0; i < lightPositions.size(); i++)
	//{
	//	shaderLightingPass->SetUniformVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
	//	shaderLightingPass->SetUniformVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
	//
	//	// Update attenuation parameters and calculate radius
	//	const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
	//	const GLfloat linear = 0.7;
	//	const GLfloat quadratic = 1.8;
	//	shaderLightingPass->SetUniform1f(("lights[" + std::to_string(i) + "].Linear").c_str(), linear);
	//	shaderLightingPass->SetUniform1f(("lights[" + std::to_string(i) + "].Quadratic").c_str(), quadratic);
	//}
	//shaderLightingPass->SetUniformVec3("viewPos", camera.GetPosition());
	
	// Finally render quad
	//RenderQuad();


	glutPostRedisplay();
	glutSwapBuffers();

	camera.UpdateCameraPosition();
}


void init();
void reshape(int width, int height);
void keyboard(unsigned char key, int xpos, int ypos);
void mousebutton(int button, int state, int x, int y);
void mousemovement(int xpos, int ypos);


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	init();

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Init GLEW failed\n";
		return -1;
	}


	shaderGeometryPass = new Shader("shader_file/DeferredShading/deferred_geometry.vs", "shader_file/DeferredShading/deferred_geometry.fs");
	shaderLightingPass = new Shader("shader_file/DeferredShading/deferred_shading.vs", "shader_file/DeferredShading/deferred_shading.fs");

	test = new Shader("shader_file/vshader21.glsl", "shader_file/fshader21.glsl");

	//shaderLightingPass->use();
	//shaderLightingPass->SetUniform1i("gPosition", 0);
	//shaderLightingPass->SetUniform1i("gNormal", 1);
	//shaderLightingPass->SetUniform1i("gAlbedoSpec", 2);
	//
	//
	//objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	//objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
	//objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
	//objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
	//objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
	//objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
	//objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
	//objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
	//objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));
	//
	//srand(13);
	//for (GLuint i = 0; i < NR_LIGHTS; i++)
	//{
	//	// Calculate slightly random offsets
	//	GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
	//	GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
	//	GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
	//	lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
	//	// Also calculate random color
	//	GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
	//	GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
	//	GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
	//	lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	//}

	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};


	//deferred = new DeferredShading(SCR_WIDTH, SCR_HEIGHT);
	//cyborg = new Model("Model/cyborg/cyborg.obj", shaderGeometryPass,
	//	{ "Model/cyborg/cyborg_diffuse.png","Model/cyborg/cyborg_specular.png" });
	_floor = new Model(planeVertices, sizeof(planeVertices) / sizeof(planeVertices[0]),
		test, "Image/wood.png");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutPassiveMotionFunc(mouse);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemovement);

	glutMainLoop();

	return 0;
}

void init()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	window = glutCreateWindow("Emmm");
	glEnable(GL_DEPTH_TEST);

	// Hide the cursor
	glutSetCursor(GLUT_CURSOR_NONE);
}

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