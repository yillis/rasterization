#ifndef SHADRE__H
#define SHADER__H
#pragma once

#include <array>
#include <memory>

#include "Object.h"
#include "Object.h"
#include "Vector.h"

namespace wm
{
		// the barycentric is cal, the z_inter is the interpolation of v coor
		class Payload {
		public:
			std::shared_ptr<Object> object;
			Vector3f barycentric;
			Vector4f eye_pos;
			float z_inter;
		public:
			Payload(const std::shared_ptr<Object>& _object, const Vector3f& _barycentric, const Vector4f& _eye_pos, float _z_inter)
				:object(_object), barycentric(_barycentric), eye_pos(_eye_pos), z_inter(_z_inter)
			{}
		};

	namespace shader {
		inline Vector3f normal_fragment_shader(const Payload& payload)
		{
			auto normal = math::interpolate3d(payload.object->n, payload.barycentric, payload.object->vex, payload.z_inter);
			normal = (normal + Vector4f(1.f, 1.f, 1.f, 0.f)) * 0.5f;
			return Vector3f(1.f - normal[0], 1.f - normal[1], 1.f - normal[2]);
		}

		inline Vector3f texture_fragment_shader(const Payload& payload)
		{
			std::array<Vector3f, 3> color;
			for (int i = 0; i < 3; ++i) {
				color[i] = math::to_vec3f(payload.object->texture->get_color(payload.object->u[i], payload.object->v[i]));
			}
			return math::interpolate3d(color, payload.barycentric, payload.object->vex, payload.z_inter);
		}
		
		inline Vector3f phone_texture_fragment_shader(const Payload& payload)
		{
			//std::array<Color, 3> color;
			//for (int i = 0; i < 3; ++i) {
			//	color[i] = payload.object->texture->get_color(payload.object->u[i], payload.object->v[i]);
			//}
			//Color res = math::interpolate3d(color, payload.barycentric, payload.object->vex, payload.z_inter);

			////Vector3f
			//return res;
			return Vector3f();
		}
	}
}

#endif // !SHADRE__H
