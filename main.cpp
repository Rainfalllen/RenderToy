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

using namespace std;


const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
//Camera camera(glm::vec3(10.0f, 0.0f, 0.0f), 0,0, glm::vec3(0, 1.0f, 0));
float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
bool firstMouse = true;

Shader* shader;
Shader* light_shader;
Shader* light_maps_shader;

Model* _model;
Model* _model2;
GLint window;

Light* l1;
Light* l2;



//初始化
void init() 
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	window = glutCreateWindow("Emmm");
	glEnable(GL_DEPTH_TEST);

	// Hide the cursor
	glutSetCursor(GLUT_CURSOR_NONE);
}

float angle = 0;
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// configure transformation matrices
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	//glm::mat4 view = camera.GetViewMatrixLookAtTarget();
	glm::mat4 view = camera.GetViewMatrix();
	

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -.5f, 0.0f));
	//model = glm::rotate(model, glm::radians(angle+=0.2f), glm::vec3(0.0, 1.0, 0));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));



	light_maps_shader->use();
	light_maps_shader->SetUniformMat4("projection", projection);
	light_maps_shader->SetUniformMat4("view", view);
	light_maps_shader->SetUniformMat4("model", model);

	//light_maps_shader->SetUniformVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	l1->SetLightInfoInShader(light_maps_shader, &camera);
	light_maps_shader->SetUniform1f("material.shininess", 32.0f);

	//绘制obj模型
	_model->Draw(light_maps_shader);

	//// 绕某个点旋转
	//model = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(angle/2), glm::vec3(0.0, 1.0, 0));
	//model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	//
	//
	//shader->SetUniformMat4("model", model);
	//
	//_model2->Draw(shader);

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

	case 'r':
	case 'R':
		glm::mat4 view = camera.GetViewMatrixLookAtTarget();
		shader->use();
		shader->SetUniformMat4("view", view);
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

// 箱子的数据
float cube_vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

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
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

int main(int argc, char* argv[])
{

	l1 = new Light(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));
	//l2 = new PointLight(glm::vec3(2, 0, 0), glm::vec3(0, 0, 0));
	//l1 = new SpotLight(glm::vec3(2, 0, 0), glm::vec3(0, 0, 0));
	//cout << ((SpotLight*)l1)->constant << endl;


	glutInit(&argc, argv);
	init();

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Init GLEW failed\n";
		return -1;
	}

	shader = new Shader("shader_file/vshader21.glsl", "shader_file/fshader21.glsl");
	light_shader = new Shader("shader_file/basic_lighting.vs", "shader_file/basic_lighting.fs");
	//light_maps_shader = new Shader("shader_file/lighting_maps.vs", "shader_file/lighting_maps.fs");
	light_maps_shader = new Shader("shader_file/lighting_maps.vs", "shader_file/lighting_maps.fs");

	l1->SetLightInfoInShader(light_maps_shader, &camera);

	_model = new Model(cube_vertices, sizeof(cube_vertices) / sizeof(cube_vertices[0]),
		light_maps_shader, { "Image/container2.png", "Image/container2_specular.png"});
	//_model = new Model(cube_vertices, sizeof(cube_vertices) / sizeof(cube_vertices[0]),
	//	light_maps_shader, { "Image/container2.png", "Image/container2_specular.png","Image/matrix.jpg" });
	//_model = new Model("Model/rock.obj", light_shader, "Model/rock.jpg");
	//_model = new Model("F:\\Model file\\nanosuit\\nanosuit.obj", shader,"F:\\Model file\\nanosuit\\hand_showroom_spec.png");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutPassiveMotionFunc(mouse);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemovement);

	glutMainLoop();

	delete shader;
	delete light_shader;
	delete light_maps_shader;
	delete _model;
	delete l1;

	return 0;
}