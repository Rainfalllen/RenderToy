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

Shader* skybox_shader;
Shader* screen_shader;
DirectionalLight* dirLight;
PointLight* pointLights;
SpotLight* spotLight;
Skybox* skybox;
PostProcessing* postProcess;
GLWindow* glWindow;

Shader* blinn_phong_shader;
Model* _floor;
Model* _box;
Shadow* shadow;
Shader* shadow_mapping_shader;

float cube_vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
};

// positions all containers
glm::vec3 cubePositions[] =
{
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 boxPos[]
{
	glm::vec3(0.0f,  1.5f,  0.0f),
	glm::vec3(2.0f,  0.0f, 0.0f),
	glm::vec3(-1.5f, 0.0f, 2.5f)
};
// positions of the point lights
glm::vec3 pointLightPositions[] =
{
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
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

bool blinn = true;

float angle = 0;
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 lightPos = glm::vec3(1.5f, 3, -3);

	// 第一次渲染，在光的空间坐标系下
	shadow->Begin(lightPos);
	shadow->depth_shader->SetUniformMat4("model", glm::mat4(1.0));
	_floor->Draw(shadow->depth_shader, false);
	for (int i = 0; i < 3; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, boxPos[i]);
		shadow->depth_shader->SetUniformMat4("model", model);
		_box->Draw(shadow->depth_shader, false);
	}
	shadow->End();


	// 第二次渲染，在摄像机空间下
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	
	shadow_mapping_shader->use();
	shadow_mapping_shader->SetUniformMat4("projection", projection);
	shadow_mapping_shader->SetUniformMat4("view", view);
	shadow_mapping_shader->SetUniformMat4("model", glm::mat4(1.0));
	shadow_mapping_shader->SetUniformMat4("lightSpaceMatrix", shadow->lightSpaceMatrix);

	shadow_mapping_shader->SetUniformVec3("viewPos", camera.GetPosition());
	shadow_mapping_shader->SetUniformVec3("lightPos", lightPos);

	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, shadow->depthMap);


	_floor->Draw(shadow_mapping_shader);
	for (int i = 0; i < 3; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, boxPos[i]);
		shadow_mapping_shader->SetUniformMat4("model", model);
		_box->Draw(shadow_mapping_shader);
	}


	// Draw skybox
	//view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	//skybox->SetShaderInfo(projection, view);
	//skybox->Draw();

	//postProcess->End(screen_shader);

	glutPostRedisplay();
	glutSwapBuffers();

	camera.UpdateCameraPosition();
}


void reshape(int width, int height);
void keyboard(unsigned char key, int xpos, int ypos);
void mousebutton(int button, int state, int x, int y);
void mousemovement(int xpos, int ypos);

vector<string>faces = {
		"Image/skybox/RiverSide/riverside_east.BMP",
		"Image/skybox/RiverSide/riverside_west.BMP",
		"Image/skybox/RiverSide/riverside_up.BMP",
		"Image/skybox/RiverSide/riverside_down.BMP",
		"Image/skybox/RiverSide/riverside_north.BMP",
		"Image/skybox/RiverSide/riverside_south.BMP" };

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glWindow = new GLWindow(WIDTH, HEIGHT);

	skybox = new Skybox(faces);
	shadow = new Shadow(WIDTH, HEIGHT);

	shadow_mapping_shader = new Shader("shader_file/Shadow/shadow_mapping.vs", "shader_file/Shadow/shadow_mapping.fs");
	shadow_mapping_shader->use();
	shadow_mapping_shader->SetUniform1i("shadowMap", 16);

	_floor = new Model(planeVertices, sizeof(planeVertices)/sizeof(planeVertices[0]), 
		shadow_mapping_shader, { "Image/wood.png","Image/white.jpg" });
	_box = new Model(cube_vertices, sizeof(cube_vertices) / sizeof(cube_vertices[0]),
		shadow_mapping_shader,
		{ "Image/container2.png","Image/container2_specular.png"});

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
		blinn = !blinn;
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