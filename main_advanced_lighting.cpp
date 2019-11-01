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

using namespace std;

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
bool firstMouse = true;

Shader* shader;
Shader* light_shader;
Shader* multiple_lights_shader;
Shader* skybox_shader;
Shader* screen_shader;

Model* _box;
Model* cyborg;

DirectionalLight* dirLight;
PointLight* pointLights;
SpotLight* spotLight;

Skybox* skybox;
PostProcessing* postProcess;
GLWindow* glWindow;

Shader* blinn_phong_shader;
Model* _floor;

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
	//postProcess->Begin();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	
	blinn_phong_shader->use();
	blinn_phong_shader->SetUniformMat4("projection", projection);
	blinn_phong_shader->SetUniformMat4("view", view);
	blinn_phong_shader->SetUniformMat4("model", glm::mat4(1.0));

	blinn_phong_shader->SetUniformVec3("viewPos", camera.GetPosition());
	blinn_phong_shader->SetUniformVec3("lightPos", glm::vec3(0,1,-3));
	blinn_phong_shader->SetUniform1i("blinn", blinn);

	_floor->Draw(blinn_phong_shader);

	//multiple_lights_shader->use();
	//multiple_lights_shader->SetUniformMat4("projection", projection);
	//multiple_lights_shader->SetUniformMat4("view", view);

	//dirLight->SetLightInfoInShader(multiple_lights_shader, &camera);
	//for (int i = 0; i < 4; i++) 
	//{
	//	pointLights[i].SetLightInfoInShader(multiple_lights_shader, &camera);
	//}
	//spotLight->SetLightInfoInShader(multiple_lights_shader, &camera);


	for (int i = 0; i < 3; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, boxPos[i]);
		//float angle = 20.0f * i;
		//model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		blinn_phong_shader->SetUniformMat4("model", model);
		//blinn_phong_shader->SetUniform1f("material.shininess", 32.0f);
	
		_box->Draw(blinn_phong_shader);
	}

	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//multiple_lights_shader->SetUniformMat4("model", model);
	//multiple_lights_shader->SetUniform1f("material.shininess", 32.0f);
	//cyborg->Draw(multiple_lights_shader);

	// Draw skybox
	//skybox_shader->use();
	//view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	//skybox_shader->SetUniformMat4("projection", projection);
	//skybox_shader->SetUniformMat4("view", view);
	//skybox->Draw(skybox_shader);

	//postProcess->End(screen_shader);

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

	//dirLight = new DirectionalLight("dirLight", glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(30, 0, 0));
	//pointLights = new PointLight[4]
	//{	PointLight("pointLights[0]", pointLightPositions[0], glm::vec3(0, 0, 0)),
	//	PointLight("pointLights[1]", pointLightPositions[1], glm::vec3(0, 0, 0)),
	//	PointLight("pointLights[2]", pointLightPositions[2], glm::vec3(0, 0, 0)),
	//	PointLight("pointLights[3]", pointLightPositions[3], glm::vec3(0, 0, 0)) };
	//spotLight = new SpotLight("spotLight", glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0, 0, 0));
	blinn_phong_shader = new Shader("shader_file/lighting_maps.vs", "shader_file/Blinn_Phong.fs");
	multiple_lights_shader = new Shader("shader_file/lighting_maps.vs", "shader_file/multiple_lights.fs");
	
	screen_shader = new Shader("shader_file/PostProcessing/framebuffer.vs", "shader_file/PostProcessing/grayscale.fs");

	_floor = new Model(planeVertices, sizeof(planeVertices), blinn_phong_shader, "Image/wood.png");

	_box = new Model(cube_vertices, sizeof(cube_vertices) / sizeof(cube_vertices[0]),
		blinn_phong_shader, { "Image/container2.png", "Image/container2_specular.png"});

	//cyborg = new Model("Model/cyborg/cyborg.obj",
	//	multiple_lights_shader, { "Model/cyborg/cyborg_diffuse.png", "Model/cyborg/cyborg_specular.png" });
	
	skybox = new Skybox();
	postProcess = new PostProcessing(WIDTH, HEIGHT, screen_shader);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemovement);

	glutMainLoop();

	//delete shader;
	//delete light_shader;
	delete multiple_lights_shader;
	delete skybox_shader;
	//delete _box;

	//delete dirLight;
	//delete[]pointLights;
	//delete spotLight;

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