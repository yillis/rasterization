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
			float z_inter;
		public:
			Payload(const std::shared_ptr<Object>& _object, const Vector3f& _barycentric, float _z_inter)
				:object(_object), barycentric(_barycentric), z_inter(_z_inter)
			{}
		};

	namespace shader {
		inline Color normal_fragment_shader(const Payload& payload)
		{
			auto normal = math::interpolate(payload.object->n, payload.barycentric, payload.object->vex, payload.z_inter);
			normal = (normal + Vector4f(1.f, 1.f, 1.f, 0.f)) * 0.5f;
			return Color(round(normal[0] * 255), round(normal[1] * 255), round(normal[2] * 255));
		}

		inline Color texture_fragment_shader(const Payload& payload)
		{
			std::array<Color, 3> color;
			for (int i = 0; i < 3; ++i) {
				color[i] = payload.object->texture->get_color(payload.object->u[i], payload.object->v[i]);
			}
			return math::interpolate(color, payload.barycentric, payload.object->vex, payload.z_inter);
		}
	}
}

#endif // !SHADRE__H
