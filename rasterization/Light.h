#ifndef LIGHT__h
#pragma once

#include "Vector.h"
namespace wm {

	class Light {
	public:
		Vector4f pos;
		Vector3f intensity;
	};

}
#endif // !LIGHT__h
