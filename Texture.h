#ifndef TEXTURE_H
#define TEXTURE_H

typedef unsigned int uint;

#include <vector>
#include <string>
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture
{
public:
	enum ENUM_TYPE
	{
		ENUM_TYPE_NOT_VALID,
		ENUM_TYPE_2D,
		ENUM_TYPE_CUBE_MAP,
		ENUM_TYPE_2D_DYNAMIC,
	};

	enum class MAG_FILTER 
	{
		NEAREST,
		LINEAR
	};

	Texture();
	Texture(uint ID, ENUM_TYPE type);
	Texture(ENUM_TYPE type);
	Texture(uint width, uint height, const float * data, uint dataType, uint srcFormat, uint internalFormat, MAG_FILTER magFilter = MAG_FILTER::NEAREST);
	Texture(const std::vector<std::string> & skybox);
	//Texture(const std::vector<std::string> & skyboxImgs);
	Texture(const std::string & path, bool flip = false, bool gammaCorrection = false);


	bool Load(const std::vector<std::string> & skybox);
	bool Load(const std::string & path, bool flip = false, bool gammaCorrection = false);

	bool GenBufferForCubemap(uint width, uint height);
	bool GenMipmap();

	bool Use(uint id = 0) const;
	void Bind() const;
	void UnBind() const;

	const uint & GetID() const { return ID; }
	ENUM_TYPE GetType() const { return type; }
	bool IsValid() const;

	void Free();

	static const Texture InValid;
private:
	static uint Type2GL(ENUM_TYPE type);
	static std::string Type2Str(ENUM_TYPE type);

	uint ID;
	ENUM_TYPE type;
};


const Texture Texture::InValid(0, ENUM_TYPE_NOT_VALID);

Texture::Texture()
	: ID(0), type(ENUM_TYPE_NOT_VALID) { }

Texture::Texture(uint ID, ENUM_TYPE type)
	: ID(ID), type(type) { }

Texture::Texture(ENUM_TYPE type)
	: Texture(0, type) { }


Texture::Texture(uint width, uint height, const float * data, uint dataType, 
	uint srcFormat, uint internalFormat, MAG_FILTER magFilter) 
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, srcFormat, dataType, data);
	//glGenerateMipmap(GL_TEXTURE_2D);
	uint glMagFilter = magFilter == MAG_FILTER::NEAREST ? GL_NEAREST : GL_LINEAR;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	type = ENUM_TYPE_2D;
}

Texture::Texture(const std::string & path, bool flip, bool gammaCorrection) 
{
	type = ENUM_TYPE_NOT_VALID;
	Load(path, flip, gammaCorrection);
}

Texture::Texture(const std::vector<std::string> & skybox) 
{
	type = ENUM_TYPE_NOT_VALID;
	Load(skybox);
}

bool Texture::IsValid() const 
{
	return ID != 0 && type != ENUM_TYPE_NOT_VALID;
}

bool Texture::Load(const std::vector<std::string> & skybox) 
{
	if (IsValid()) 
	{
		printf("Error: The texture is valid already.\n");
		return false;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	// loads a cubemap texture from 6 individual texture faces
	// order:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front) 
	// -Z (back)
	// -------------------------------------------------------
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	for (uint i = 0; i < skybox.size(); i++)
	{
		unsigned char *data = stbi_load(skybox[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			printf("Cubemap texture failed to load at path: %s\n", skybox[i].c_str());
			type = ENUM_TYPE_NOT_VALID;
			return false;
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	type = ENUM_TYPE_CUBE_MAP;
	UnBind();
	return true;
}


bool Texture::Load(const std::string & path, bool flip, bool gammaCorrection) 
{
	if (IsValid()) 
	{
		printf("Error: The texture is valid already.\n");
		return false;
	}

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(flip);
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (!data) 
	{
		printf("Error: Texture [%s] load failed\n", path.c_str());
		type = ENUM_TYPE_NOT_VALID;
		return false;
	}

	// 设置格式
	GLenum internalFormat;
	GLenum dataFormat;
	if (nrChannels == 1) 
	{
		internalFormat = GL_RED;
		dataFormat = GL_RED;
	}
	else if (nrChannels == 3)
	{
		internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
		dataFormat = GL_RGB;
	}
	else if (nrChannels == 4)
	{
		internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
		dataFormat = GL_RGBA;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	/*
	@1 纹理目标
	@2 多级渐远纹理的级别 （0 为基本级别)
	@3 纹理格式
	@4 width
	@5 height
	@6 0 (历史遗留问题)
	@7 源图格式
	@8 源图类型
	@9 图像数据
	*/
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	type = ENUM_TYPE_2D;
	UnBind();
	return true;
}


bool Texture::Use(uint id) const 
{
	if (!IsValid()) 
	{
		printf("Error:Texture::Use:\n"
			"\t""use a invalid texture\n");
		return false;
	}

	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(Type2GL(type), ID);
	return true;
}

uint Texture::Type2GL(ENUM_TYPE type) {
	switch (type)
	{
	case ENUM_TYPE_NOT_VALID:
		return 0;
	case ENUM_TYPE_2D:
	case ENUM_TYPE_2D_DYNAMIC:
		return GL_TEXTURE_2D;
	case ENUM_TYPE_CUBE_MAP:
		return GL_TEXTURE_CUBE_MAP;
	default:
		return 0;
	}
}

std::string Texture::Type2Str(ENUM_TYPE type) 
{
	switch (type)
	{
	case ENUM_TYPE_NOT_VALID:
		return "ENUM_TYPE_NOT_VALID";
	case ENUM_TYPE_2D:
		return "ENUM_TYPE_2D";
	case ENUM_TYPE_CUBE_MAP:
		return "ENUM_TYPE_CUBE_MAP";
	case ENUM_TYPE_2D_DYNAMIC:
		return "ENUM_TYPE_2D_DYNAMIC";
	default:
		return "UNKNOWN_TYPE";
	}
}

void Texture::Bind() const 
{
	if (!IsValid()) return;
	glBindTexture(Type2GL(type), ID);
}

void Texture::UnBind() const 
{
	if (!IsValid()) return;
	glBindTexture(Type2GL(type), 0);
}

bool Texture::GenBufferForCubemap(uint width, uint height) 
{
	if (type != ENUM_TYPE_CUBE_MAP) {
		printf("ERROR::Texture::GenBufferForCubemap:\n"
			"\t""type is not ENUM_TYPE_CUBE_MAP\n");
		return false;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	for (uint i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	type = ENUM_TYPE_CUBE_MAP;
	UnBind();

	return true;
}

bool Texture::GenMipmap() 
{
	if (!IsValid()) 
	{
		printf("ERROR::Texture::GenMipmap:\n"
			"\t""texture is not valid\n");
		return false;
	}

	Bind();
	auto glType = Type2GL(type);
	glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(glType);
	return true;
}

void Texture::Free() {
	if (!IsValid())
		return;

	glDeleteTextures(1, &ID);
	ID = -1;
	type = ENUM_TYPE_NOT_VALID;
}





#endif

