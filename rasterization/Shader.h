#ifndef SHADRE__H
#define SHADER__H
#pragma once

#include "Object.h"
#include "Vector.h"

namespace wm
{
	class Payload {
		// ToAdd: vex
		Vector3f color;
		Vector3f normal;
	};

	inline Color normal_shader(const Payload& payload)
	{

		return Color();
	}
}

#endif // !SHADRE__H
