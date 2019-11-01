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

GLWindow* glWindow;

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
int nrRows = 8;
int nrColumns = 8;
float spacing = 2.5;

void renderSphere();

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

	// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row <= nrRows; ++row)
	{
		pbr_shader->SetUniform1f("metallic", (float)row / (float)nrRows);
		for (int col = 0; col <= nrColumns; ++col)
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
			renderSphere();
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
	pbr_shader = new Shader("shader_file/PBR/pbr.vs", "shader_file/PBR/pbr.fs");
	pbr_shader->use();
	pbr_shader->SetUniformVec3("albedo", glm::vec3(0.5f, 0.5f, 0.5f));
	pbr_shader->SetUniform1f("ao", 1.0f);

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

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}
