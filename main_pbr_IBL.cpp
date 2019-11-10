#include<GL/glew.h>
#include<GL/freeglut.h>
#include<GLFW/glfw3.h>

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

#include "Primitive/Sphere.h"
#include "Primitive/Cube.h"
#include "Primitive/Quad.h"

#include "Environment.h"

using namespace std;


const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -30.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
//Camera camera(glm::vec3(10.0f, 0.0f, 0.0f), 0,0, glm::vec3(0, 1.0f, 0));
float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
bool firstMouse = true;


Shader* pbr_shader;
//Shader* equirectangularToCubemapShader;
//Shader* backgroundShader;
//Shader* irradianceShader;
//Shader* brdfShader;
//Shader* prefilterShader;

GLWindow* glWindow;
Environment* environment;

//unsigned int captureFBO;
//unsigned int captureRBO;
//unsigned int hdrTexture;
//unsigned int envCubemap;
//unsigned int irradianceMap;
//unsigned int prefilterMap;
//unsigned int brdfLUTTexture;

// lights
// -------------------------------------
glm::vec3 lightPositions[] = {
	glm::vec3(-10.0f,  10.0f, -10.0f),
	glm::vec3(10.0f,  10.0f, -10.0f),
	glm::vec3(-10.0f, -10.0f, -10.0f),
	glm::vec3(10.0f, -10.0f, -10.0f),
};

glm::vec3 lightColors[] = {
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f)
};
int nrRows = 7;
int nrColumns = 7;
float spacing = 2.5;


float angle = 0;
void display()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	
	pbr_shader->use();
	pbr_shader->SetUniformMat4("projection", projection);
	pbr_shader->SetUniformMat4("view", view);
	pbr_shader->SetUniformVec3("camPos", camera.GetPosition());

	// bind pre-computed IBL data
	environment->Use();

	// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row < nrRows; ++row)
	{
		pbr_shader->SetUniform1f("metallic", (float)row / (float)nrRows);
		for (int col = 0; col < nrColumns; ++col)
		{
			// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
			// on direct lighting.
			pbr_shader->SetUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.025f, 1.0f));

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				( -col + (nrColumns / 2)) * spacing,
				( -row + (nrRows / 2)) * spacing,
				0.0f
			));
			pbr_shader->SetUniformMat4("model", model);
			Sphere::Draw();
		}
	}

	// render light source (simply re-render sphere at light positions)
	// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
	// keeps the codeprint small.
	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		glm::vec3 newPos = lightPositions[i];
		//glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		pbr_shader->SetUniformVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		pbr_shader->SetUniformVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

		model = glm::mat4(1.0f);
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.1f));
		pbr_shader->SetUniformMat4("model", model);
		Sphere::Draw();
	}

	// render skybox (render as last to prevent overdraw)
	environment->RenderSkybox(projection, view);

	//--------------------------------------------------------------------------------------
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

	environment = new Environment(WIDTH, HEIGHT);

	pbr_shader = new Shader("shader_file/PBR/pbr.vs", "shader_file/PBR/pbr_IBL.fs");

	pbr_shader->use();
	pbr_shader->SetUniform1i("irradianceMap", 0);
	pbr_shader->SetUniform1i("prefilterMap", 1);
	pbr_shader->SetUniform1i("brdfLUT", 2);
	pbr_shader->SetUniformVec3("albedo", glm::vec3(0.5f, 0.5f, 0.5f));
	pbr_shader->SetUniform1f("ao", 1.0f);

	// -----------------------------------------------------------
	glWindow->SetRenderScene(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemovement);

	glutMainLoop();

	delete glWindow;
	delete environment;
	delete pbr_shader;

	return 0;
}


void reshape(int width, int height)
{
	//glViewport(0, 0, width, height);
	float temp = (float)width / (float)height;
	float scale = (float)WIDTH / (float)HEIGHT;
	
	if (temp > scale)
		glViewport(0, 0, width, width * 1.0 / scale);
	else
		glViewport(0, 0, height * scale, height);
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
