#ifndef PIXEL__H
#pragma once

#include <vector>

#include "Vector.h"

namespace wm {

	class Pixel {
	private:
		int samples_;
	public:
		std::vector<Vector3f> color;
		std::vector<float> depth;
		Pixel(int samples = 1, const Vector3f& background_color = { 0.f,0.f,0.f }) 
			:samples_(samples),
			color(std::vector<Vector3f>(samples, background_color)),
			depth(std::vector<float>(samples, std::numeric_limits<float>::max()))
		{}
		Vector3f get_color() const
		{
			Vector3f res = color[0];
			for (int i = 1; i < samples_; ++i) {
				res = math::blend_color(res, color[i]);
			}
			return res;
		}
		Vector3f operator()() const { return get_color(); }
	};

}

#endif // !PIXEL__H
