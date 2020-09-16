#ifndef BOUND__H
#define BOUND__H
#pragma once

#include <functional>

#include "Vector.h"

namespace wm {

	class Bound {
	public:
		Vector3f lower;
		Vector3f upper;

	public:
		Bound() :lower(), upper() {}
		Bound(const Vector3f& _lower, const Vector3f& _upper) :lower(_upper), upper(_upper) {}

	private:
		// ToDo: move to Vector.h
		Vector3f vec_max(const Vector3f& a, const Vector3f& b) const {
			return Vector3f(std::fmax(a[0], b[0]), std::fmax(a[1], b[1]), std::fmax(a[2], b[2]));
		}
		Vector3f vec_min(const Vector3f& a, const Vector3f& b) const {
			return Vector3f(std::fmin(a[0], b[0]), std::fmin(a[1], b[1]), std::fmin(a[2], b[2]));
		}

	public:
		Bound operator|(const Bound& rhs) const {
			return Bound(vec_min(lower, rhs.lower), vec_max(upper, rhs.upper));
		}
		Bound operator&(const Bound& rhs) const {
			return Bound(vec_max(lower, rhs.lower), vec_min(upper, rhs.upper));
		}
	public:
		Vector3f center() const {
			return (lower + upper) * 0.5f;
		}
	};

} // !namespace wm
#endif // !BOUND__H
