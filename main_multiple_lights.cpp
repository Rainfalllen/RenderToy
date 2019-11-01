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
#include "GLWindow.h"

using namespace std;


const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, -7.0f), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0, 1.0f, 0));
//Camera camera(glm::vec3(10.0f, 0.0f, 0.0f), 0,0, glm::vec3(0, 1.0f, 0));
float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
bool firstMouse = true;

Shader* shader;
Shader* light_shader;
Shader* multiple_lights_shader;
Shader* skybox_shader;

Model* box;
Model* _model;
GLWindow* window;

DirectionalLight* dirLight;
PointLight* pointLights;
SpotLight* spotLight;

Skybox* skybox;


// positions of the point lights
glm::vec3 pointLightPositions[] =
{
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};


float angle = 0;
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	
	multiple_lights_shader->use();
	multiple_lights_shader->SetUniformMat4("projection", projection);
	multiple_lights_shader->SetUniformMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(180.0f + (angle+=0.2f)), glm::vec3(0.0f, 1.0f, 0.0f));
	multiple_lights_shader->SetUniformMat4("model", model);
	multiple_lights_shader->SetUniform1f("material.shininess", 32.0f);

	dirLight->SetLightInfoInShader(multiple_lights_shader, &camera);
	for (int i = 0; i < 4; i++) 
	{
		pointLights[i].SetLightInfoInShader(multiple_lights_shader, &camera);
	}
	spotLight->SetLightInfoInShader(multiple_lights_shader, &camera);

	_model->Draw();



	// Draw skybox
	view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	skybox->SetShaderInfo(projection, view);
	skybox->Draw();


	glutPostRedisplay();
	glutSwapBuffers();

	camera.UpdateCameraPosition();
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


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	window = new GLWindow(WIDTH, HEIGHT);

	dirLight = new DirectionalLight("dirLight", glm::vec3(0, 0, 1));
	pointLights = new PointLight[4]
	{	PointLight("pointLights[0]", pointLightPositions[0], glm::vec3(0, 0, 0)),
		PointLight("pointLights[1]", pointLightPositions[1], glm::vec3(0, 0, 0)),
		PointLight("pointLights[2]", pointLightPositions[2], glm::vec3(0, 0, 0)),
		PointLight("pointLights[3]", pointLightPositions[3], glm::vec3(0, 0, 0)) };
	spotLight = new SpotLight("spotLight", glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0, 0, 0));

	multiple_lights_shader = new Shader("shader_file/lighting_maps.vs", "shader_file/multiple_lights.fs");

	_model = new Model("Resource/Model/cyborg/cyborg.obj", multiple_lights_shader, 
		{ "Resource/Model/cyborg/cyborg_diffuse.png","Resource/Model/cyborg/cyborg_specular.png" });
	_model->SetShaderUniforms({"material.diffuseTex","material.specularTex"});
	
	skybox = new Skybox();

	window->SetRenderScene(display);
	//glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemovement);

	glutMainLoop();


	delete multiple_lights_shader;
	delete _model;

	delete skybox;
	delete window;

	delete dirLight;
	delete[]pointLights;
	delete spotLight;

	return 0;
}