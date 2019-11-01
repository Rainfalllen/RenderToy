#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<GL/glew.h>
#include<GL/freeglut.h>

#include"Shader.h"
#include "Texture.h"

#include <string>
#include<vector>
using std::vector;
using std::string;

#pragma warning(disable : 4996)
class Model 
{
public:
	Model(const char* modelPath, Shader* shader, const char* texturePath = nullptr)
	{
		ModelPath = modelPath;
		TexturePath[0] = texturePath;
		TextureCount = 1;
		if (LoadObj(modelPath, m_positions, m_uvs, m_normals))
		{
			SetupModel(shader);
		}
		else
		{
			printf("Model building has failed...");
		}
	}

	Model(const char* modelPath, Shader* shader, vector<const char*> texturePaths)
	{
		ModelPath = modelPath;

		TextureCount = texturePaths.size();
		for (int i = 0; i < TextureCount; i++)
		{
			TexturePath[i] = texturePaths[i];
		}

		if (LoadObj(modelPath, m_positions, m_uvs, m_normals))
		{
			SetupModel(shader);
		}
		else
		{
			printf("Model building has failed...");
		}
	}

	Model(GLfloat* arr, int n, Shader* shader, vector<const char*> texturePaths)
	{
		for (int i = 0; i < n; i++)
		{
			vertices.push_back(arr[i]);
		}

		TextureCount = texturePaths.size();

		for (int i = 0; i < TextureCount; i++)
		{
			TexturePath[i] = texturePaths[i];
		}

		SetupModel(shader);
	}

	Model(GLfloat* arr, int n, Shader* shader, const char* texturePath = nullptr)
	{
		for (int i = 0; i < n; i++)
		{
			vertices.push_back(arr[i]);
		}

		TexturePath[0] = texturePath;
		TextureCount = 1;
		//printf("%s\n", TexturePath[0]);
		SetupModel(shader);
	}

	~Model()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		for (int i = 0; i < TextureCount; i++)
		{
			texture[i]->Free();
		}
		delete[]texture;
	}

	vector<glm::vec3> m_positions;
	vector<glm::vec2> m_uvs;
	vector<glm::vec3> m_normals;

	bool LoadObj(const char*path,
		vector<glm::vec3>&positions,
		vector<glm::vec2>&uvs,
		vector<glm::vec3>&normals)
	{
		printf("Loading OBJ file %s...\n", path);

		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;

		FILE* file;
		//errno_t err = fopen_s(&file, path, "r");
		if (errno_t err = fopen_s(&file, path, "r") != 0)
		{
			printf("The file %s was not opened\n", path);
			return false;
		}

		while (true)
		{
			//char lineHeader[1024];
			//int res = fscanf_s(file, "%s", lineHeader);
			//if (res == EOF)
			//	break;

			if (feof(file))break;
			char lineHeader[128];
			fscanf(file, "%s", lineHeader);


			if (strcmp(lineHeader, "v") == 0)
			{
				//printf("Get v\n");
				glm::vec3 vertex(0);
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				//printf("Get vn\n");
				glm::vec3 normal(0);
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				//printf("Get vt\n");
				glm::vec2 uv(0);
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				uv.y = -uv.y;
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				//printf("Get f\n");
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2]);

				if (matches != 9) {
					printf("File can't be read the parser :-( Try exporting with other options\n");
					return false;
				}

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
			else
			{
				// Probably a comment, eat up the rest of the line  
				char stupidBuffer[1024];
				fgets(stupidBuffer, 1024, file);
			}

		}

		//For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			// Get the indices of its attributes  
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int uvIndex = uvIndices[i];
			unsigned int normalIndex = normalIndices[i];

			// Get the attributes thanks to the index  
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			glm::vec2 uv = temp_uvs[uvIndex - 1];
			glm::vec3 normal = temp_normals[normalIndex - 1];

			// Put the attributes in buffers  
			positions.push_back(vertex);
			uvs.push_back(uv);
			normals.push_back(normal);
		}

		InitVectices();
		return true;
	}

	// 已经被弃用的立即渲染模式
	void Test_Draw() {

		glBegin(GL_TRIANGLES);
		for (int i = 0; i < m_positions.size(); i++)
		{
			glNormal3f(m_normals[i].x, m_normals[i].y, m_normals[i].z);
			glVertex3f(m_positions[i].x, m_positions[i].y, m_positions[i].z);
		}
		glEnd();
	}

	void Draw()
	{
		currentShader->use();

		if (TextureCount > 0)
		{
			for (int i = 0; i < TextureCount; i++)
			{
				texture[i]->Use(i);
			}
		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}

	void Draw(Shader* shader,bool use_texture = true)
	{
		shader->use();

		if (use_texture && TextureCount > 0)
		{
			for (int i = 0; i < TextureCount; i++)
			{
				texture[i]->Use(i);
			}
		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}


	void InitVectices()
	{
		for (int i = 0; i < m_positions.size(); ++i)
		{
			vertices.push_back(m_positions[i].x);
			vertices.push_back(m_positions[i].y);
			vertices.push_back(m_positions[i].z);

			vertices.push_back(m_normals[i].x);
			vertices.push_back(m_normals[i].y);
			vertices.push_back(m_normals[i].z);

			vertices.push_back(m_uvs[i].x);
			vertices.push_back(m_uvs[i].y);
		}
	}
	

	void SetupModel(Shader* shader)
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		uniforms = {"diffuseTex","specularTex","emissionTex"};
		currentShader = new Shader();
		currentShader = shader;

		if (TextureCount > 0)
		{
			for (int i = 0; i < TextureCount&&i<uniforms.size(); i++) 
			{
				texture[i] = new Texture(TexturePath[i]);

				currentShader->use();
				currentShader->SetUniform1i(uniforms[i], i);
			}
		}

	}

	void ChangeShader(Shader* newShader)
	{
		currentShader = newShader;
	}

	void SetShaderUniforms(vector<string> _uniforms)
	{
		uniforms.clear();
		uniforms = _uniforms;

		if (TextureCount > 0)
		{
			for (int i = 0; i < TextureCount && i < uniforms.size(); i++)
			{
				texture[i]->Free();
				texture[i] = new Texture(TexturePath[i]);

				currentShader->use();
				currentShader->SetUniform1i(uniforms[i], i);
			}
		}
	}

private:

	const char* ModelPath; 
	const char* TexturePath[16];

	GLuint VAO, VBO;
	vector<GLfloat> vertices;
	Texture* texture[16];
	int TextureCount;
	Shader* currentShader;

	vector<string> uniforms;

};
