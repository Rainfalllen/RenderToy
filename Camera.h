#ifndef CAMERA_H
#define CAMERA_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
	{
		Target = target;

		Position = position;
		WorldUp = worldUp;
		Forward = glm::normalize(target - position);
		Right = glm::normalize(glm::cross(Forward, WorldUp));
		Up = glm::normalize(glm::cross(Right, Forward));

		Pitch = glm::asin(Forward.y);
		Yaw = glm::atan(Forward.x / Forward.z);

	}

	Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldUp)
	{
		Position = position;
		WorldUp = worldUp;
		Pitch = pitch;
		Yaw = yaw;
		Forward.x = glm::cos(pitch) * glm::sin(yaw);
		Forward.y = glm::sin(pitch);
		Forward.z = glm::cos(pitch) * glm::cos(yaw);
		Right = glm::normalize(glm::cross(Forward, WorldUp));
		Up = glm::normalize(glm::cross(Right, Forward));
	}


	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Forward, WorldUp);
	}

	glm::mat4 GetViewMatrixLookAtTarget()
	{
		// Camera always looks at Target
		Forward = glm::normalize(Target - Position);
		Right = glm::normalize(glm::cross(Forward, WorldUp));
		Up = glm::normalize(glm::cross(Right, Forward));
		
		Pitch = glm::asin(Forward.y);
		Yaw = glm::atan(Forward.x / Forward.z);
		//printf("%f  %f\n", Pitch, Yaw);
		//UpdateCameraVectors();
		//printf("%f  %f\n", Pitch, Yaw);
		//printf("%f  %f  %f\n", Forward.x, Forward.y,Forward.z);
		return glm::lookAt(Position, Position + Forward, WorldUp);
	}

	
	//float Zoom() { return glm::radians(45.0f); }


	void ProcessMouseMovement(float deltaX, float deltaY, bool constrainPitch)
	{
		Pitch -= deltaY * MouseSensitivity;
		Yaw -= deltaX * MouseSensitivity;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		UpdateCameraVectors();
	}

	void UpdateCameraPosition()
	{
		// update the position
		Position += Forward * speedZ * 0.1f;
		Position += Right * speedX * 0.1f;
		//Position += Up * speedY * 0.1f;
		Position += WorldUp * speedY * 0.1f;

		// reset the speed
		speedX = 0;
		speedY = 0;
		speedZ = 0;
	}

	float speedX = 0;
	float speedY = 0;
	float speedZ = 0;

	glm::vec3 GetPosition() { return Position; }

private:
	glm::vec3 Target;
	glm::vec3 Position;
	glm::vec3 Forward;
	glm::vec3 Right;
	glm::vec3 Up;
	glm::vec3 WorldUp;

	float Pitch;
	float Yaw;
	float MouseSensitivity = 0.005f;

	float MovementSpeed = 2.5f;

	void UpdateCameraVectors()
	{
		Forward.x = glm::cos(Pitch) * glm::sin(Yaw);
		Forward.y = glm::sin(Pitch);
		Forward.z = glm::cos(Pitch) * glm::cos(Yaw);
		Forward = glm::normalize(Forward);

		Right = glm::normalize(glm::cross(Forward, WorldUp));
		Up = glm::normalize(glm::cross(Right, Forward));

		//printf("%f  %f\n", Pitch, Yaw);
		//printf("%f  %f  %f\n", Forward.x, Forward.y, Forward.z);
	}
};


#endif