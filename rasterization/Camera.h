#ifndef CAMERA__H
#define CAMERA__H
#pragma once

#include "Vector.h"
namespace wm {
	class Camera {
	public:
		Vector4f eye_pos;
		Vector4f up;
		Vector4f look_at;
		float fov;
		float aspect;
		float z_near;
		float z_far;
		Camera() = default;
		Camera(Vector4f _eye_pos, Vector4f _up, Vector4f _look_at, float _fov, float _aspect, float _z_near, float _z_far) :
			eye_pos(_eye_pos), up(_up), look_at(_look_at), fov(_fov), aspect(_aspect), z_near(_z_near), z_far(_z_far) {}
	};

}

#endif // !CAMERA__H