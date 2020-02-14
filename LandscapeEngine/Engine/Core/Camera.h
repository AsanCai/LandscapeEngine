#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <math.h>

namespace LandscapeEngine {
	// Defines several possible options for camera movement. 
	// Used as abstraction to stay away from window-system specific input methods
	enum MovementDirection {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	// Default camera values
	const float CAMERA_YAW = -90.0f;
	const float CAMERA_PITCH = 0.0f;
	const float CAMERA_SPRINT = 2.0f;
	const float CAMERA_SPEED = 100.0f;
	const float CAMERA_SENSITIVITY = 0.1f;
	const float CAMERA_MAX_FOV = 60.0f;
	const float CAMERA_MIN_FOV = 1.0f;

	const float CAMERA_NEAR = 0.1f;
	const float CAMERA_FAR = 2000.0f;
	const float CAMERA_FOV = 45.0f;

	// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
	class Camera {
	public:
		// the position of camera
		glm::vec3 position;
		// the fov of camera
		float fov;
		
	public:
		// constructor with vectors
		Camera(int width, int height, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float zNear = CAMERA_NEAR, float zFar = CAMERA_FAR, float fov = CAMERA_FOV)
			: _yaw(CAMERA_YAW), _pitch(CAMERA_PITCH), _front(glm::vec3(0.0f, 0.0f, -1.0f)), _speed(CAMERA_SPEED), _sensitivity(CAMERA_SENSITIVITY) {
			_width = width;
			_height = height;
			_worldUp = worldUp;
			_near = zNear;
			_far = zFar;

			this->position = position;
			this->fov = fov;

			updateCameraVectors();
		}

		// change the width and height of camera
		void changeSize(int width, int height) {
			_width = width;
			_height = height;
		}

		// return the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 getViewMatrix() {
			return glm::lookAt(this->position, this->position + _front, _up);
		}

		// return the Projection matrix
		glm::mat4 getProjectionMatrix() {
			return glm::perspective(glm::radians(this->fov), (float)_width / (float)_height, _near, _far);
		}

		// Processes input received from any keyboard-like input system. 
		// Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void translate(MovementDirection direction, float deltaTime) {
			float velocity = _speed * deltaTime;

			switch (direction) {
			case FORWARD:
				this->position += _front * velocity;
				break;
			case BACKWARD:
				this->position -= _front * velocity;
				break;
			case LEFT:
				this->position -= _right * velocity;
				break;
			case RIGHT:
				this->position += _right * velocity;
				break;
			case UP:
				this->position += _up * velocity;
				break;
			case DOWN:
				this->position -= _up * velocity;
				break;
			default:
				break;
			}
		}

		// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
			xoffset *= _sensitivity;
			yoffset *= _sensitivity;

			_yaw += xoffset;
			_pitch += yoffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch) {
				if (_pitch > 89.0f) {
					_pitch = 89.0f;
				}
				if (_pitch < -89.0f) {
					_pitch = -89.0f;
				}
			}

			// Update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}

		// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void processMouseScroll(float yoffset) {
			float temp = this->fov - yoffset;
			this->fov = glm::clamp(temp, CAMERA_MIN_FOV, CAMERA_MAX_FOV);
		}

	private:
		/* Camera attributes */
		glm::vec3 _up;
		glm::vec3 _right;
		glm::vec3 _front;
		glm::vec3 _worldUp;

		int _width;
		int _height;
		float _near;
		float _far;

		// Euler Angles
		float _yaw;
		float _pitch;
		// Camera options
		float _speed;
		float _sensitivity;

	private:
		// Calculates the front vector from the Camera's (updated) Euler Angles
		void updateCameraVectors() {
			// Calculate the new Front vector
			glm::vec3 newFront;
			newFront.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
			newFront.y = sin(glm::radians(_pitch));
			newFront.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

			this->_front = glm::normalize(newFront);
			// Also re-calculate the Right and Up vector
			// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			this->_right = glm::normalize(glm::cross(this->_front, this->_worldUp));
			this->_up = glm::normalize(glm::cross(this->_right, this->_front));
		}
	};
}