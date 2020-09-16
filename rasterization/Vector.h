#ifndef VECTOR__H
#define VECTOR__H
#pragma once

#include <stdexcept>
#include <cmath>
#include <array>

namespace wm 
{
	// these Vector's design is to bad, it should use inherit form a base Vector class
	// but the writer is to lazy to do that ^_^

	class Vector2f;
	class Vector3f;
	class Vector4f;
	class Matrix3f;
	class Matrix4f;

	class Vector2f {
	public:
		float x;
		float y;
		float operator[](int i) const;
		float& operator[](int i);
		Vector2f operator*(float n) const;
		float operator*(const Vector2f& v) const;
		Vector2f operator-(const Vector2f& v) const;
		Vector2f operator-(void) const;
		Vector2f operator+(const Vector2f& v) const;
		Vector2f operator+(float n) const;
		Vector3f cross_product(const Vector2f& v) const;

		Vector2f() :x(0.f), y(0.f) {}
		Vector2f(float n) :x(n), y(n) {}
		Vector2f(float _x, float _y) :x(_x), y(_y) {}
	};
	class Vector3f {
	public:
		float x;
		float y;
		float z;
		float operator[](int i) const;
		float& operator[](int i);
		Vector3f operator*(float n) const;
		float operator*(const Vector3f& v) const;
		Vector3f operator-(const Vector3f& v) const;
		Vector3f operator-(void) const;
		Vector3f operator+(const Vector3f& v) const;
		Vector3f operator+(float n) const;
		Vector3f cross_product(const Vector3f& v) const;

		void standard() { if (z != 0) { x /= z, y /= z, z = 1.f; } }
		Vector2f vec2() const;

		Vector3f() :x(0.f), y(0.f), z(0.f) {}
		Vector3f(float n) :x(n), y(n), z(n) {}
		Vector3f(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}
	};

	class Vector4f {
	public:
		float x;
		float y;
		float z;
		float w;
		float operator[](int i) const;
		float& operator[](int i);
		Vector4f operator*(float n) const;
		float operator*(const Vector4f& v) const;
		Vector4f operator-(const Vector4f& v) const;
		Vector4f operator-(void) const;
		Vector4f operator+(const Vector4f& v) const;
		Vector4f operator+(float n) const;
		Vector4f cross_product3(const Vector4f& v) const; // Æë´Î×ø±ê
		

		void standard() { if (w != 0) { x /= w, y /= w, z /= w, w = 1.f; } }
		Vector3f vec3() const;
		Vector2f vec2() const;

		Vector4f() :x(0.f), y(0.f), z(0.f), w(0.f) {}
		Vector4f(float n) :x(n), y(n), z(n), w(n) {}
		Vector4f(float _x, float _y, float _z, float _w) :x(_x), y(_y), z(_z), w(_w) {}
	};

	class Matrix3f {
	public:
		Vector3f matrix[3];
		Vector3f operator[](int i) const;
		Vector3f& operator[](int i);

		Matrix3f operator*(float n) const;
		Matrix3f operator*(const Matrix3f& m) const;
		Vector3f operator*(const Vector3f& v) const;

		Matrix3f() :matrix() {}
		Matrix3f(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2) :matrix{v0, v1, v2} {}
	};

	class Matrix4f {
	public:
		Vector4f matrix[4];
		Vector4f operator[](int i) const;
		Vector4f& operator[](int i);

		Matrix4f operator*(float n) const;
		Matrix4f operator*(const Matrix4f& m) const;
		Vector4f operator*(const Vector4f& v) const;

		Matrix4f() :matrix() {}
		Matrix4f(const Vector4f& v0, const Vector4f& v1, const Vector4f& v2, const Vector4f& v3)
			:matrix{ v0, v1, v2, v3 } {}
	};

	
	inline Vector3f normalize(const Vector3f& v)
	{
		Vector3f res;
		if (v.z == 0.f) {
			res = v;
		}
		else {
			res = { v.x / v.z, v.y / v.z, 1.f };
		}
		float n = std::sqrt(res.x * res.x + res.y * res.y);
		res.x /= n, res.y /= n;
		return res;
	}
	inline Vector4f normalize(const Vector4f& v)
	{
		Vector4f res;
		if (v.w == 0.f) {
			res = v;
		}
		else {
			res = { v.x / v.w, v.y / v.w, v.z / v.w, 1.f };
		}
		float n = std::sqrt(res.x * res.x + res.y * res.y + res.z * res.z);
		res.x /= n, res.y /= n, res.z /= n;
		return res;
	}

	template<class Matrix, int size>
	inline Matrix identity()
	{
		Matrix res;
		for (int i = 0; i < size; ++i) {
			res[i][i] = 1.f;
		}
		return res;
	}
} // !namespace wm

// global operator override func ---------------------------------------------------------------
inline wm::Vector2f operator*(float n, const wm::Vector2f& v)
{
	return v * n;
}
inline wm::Vector2f operator-(float n, const wm::Vector2f& v)
{
	return -v + n;
}
inline wm::Vector3f operator*(float n, const wm::Vector3f& v)
{
	return v * n;
}
inline wm::Vector3f operator-(float n, const wm::Vector3f& v)
{
	return -v + n;
}
inline wm::Vector4f operator*(float n, const wm::Vector4f& v)
{
	return v * n;
}
inline wm::Vector4f operator-(float n, const wm::Vector4f& v)
{
	return -v + n;
}
inline wm::Matrix3f operator*(float n, const wm::Matrix3f& m)
{
	return m * n;
}
inline wm::Matrix4f operator*(float n, const wm::Matrix4f& m)
{
	return m * n;
}
// !global operator override func ---------------------------------------------------------------

// Vector2f class func
inline float wm::Vector2f::operator[](int i) const
{
	if (i < 0 || i >= 2) {
		throw std::runtime_error("index out of bounds in Vector2f.");
	}
	return *(&x + i);
}

inline float& wm::Vector2f::operator[](int i)
{
	if (i < 0 || i >= 2) {
		throw std::runtime_error("index out of bounds in Vector2f.");
	}
	return *(&x + i);
}

inline wm::Vector2f wm::Vector2f::operator*(float n) const
{
	return Vector2f(n * x, n * y);
}

inline float wm::Vector2f::operator*(const Vector2f& v) const
{
	return x * v.x + y * v.y;
}

inline wm::Vector2f wm::Vector2f::operator-(const Vector2f& v) const
{
	return Vector2f(x - v.x, y - v.y);
}

inline wm::Vector2f wm::Vector2f::operator-(void) const
{
	return Vector2f(-x, -y);
}

inline wm::Vector2f wm::Vector2f::operator+(const Vector2f& v) const
{
	return Vector2f(x + v.x, y + v.y);
}

inline wm::Vector2f wm::Vector2f::operator+(float n) const
{
	return Vector2f(x + n, y + n);
}

inline wm::Vector3f wm::Vector2f::cross_product(const Vector2f& v) const
{
	return Vector3f(0.f, 0.f, x * v.y - y * v.x);
}


// Vector3f class func ---------------------------------------------------------------
inline float wm::Vector3f::operator[](int i) const
{
	if (i < 0 || i >= 3) {
		throw std::runtime_error("index out of bounds in Vector3f.");
	}
	return *(&x + i);
}
inline float& wm::Vector3f::operator[](int i)
{
	if (i < 0 || i >= 3) {
		throw std::runtime_error("index out of bounds in Vector3f.");
	}
	return *(&x + i);
}

inline wm::Vector3f wm::Vector3f::operator*(float n) const
{
	return Vector3f(n * x, n * y, n * z);
}

inline float wm::Vector3f::operator*(const Vector3f& v) const
{
	return x * v.x + y * v.y + z * v.z;
}
inline wm::Vector3f wm::Vector3f::operator-(const Vector3f& v) const
{
	return Vector3f(x - v.x, y - v.y, z - v.z);
}
inline wm::Vector3f wm::Vector3f::operator-(void) const
{
	return Vector3f(-x, -y, -z);
}
inline wm::Vector3f wm::Vector3f::operator+(const Vector3f& v) const
{
	return Vector3f(x + v.x, y + v.y, z + v.z);
}
inline wm::Vector3f wm::Vector3f::operator+(float n) const
{
	return Vector3f(x + n, y + n, z + n);
}
inline wm::Vector3f wm::Vector3f::cross_product(const Vector3f& v) const
{
	return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}
inline wm::Vector2f wm::Vector3f::vec2() const
{
	if (z == 0) {
		return Vector2f(x, y);
	}
	return Vector2f(x / z, y / z);
}
// !Vector3f class func ---------------------------------------------------------------


// Vector4f class func ---------------------------------------------------------------
inline float wm::Vector4f::operator[](int i) const
{
	if (i < 0 || i >= 4) {
		throw std::runtime_error("index out of bounds in Vector4f.");
	}
	return *(&x + i);
}
inline float& wm::Vector4f::operator[](int i)
{
	if (i < 0 || i >= 4) {
		throw std::runtime_error("index out of bounds in Vector4f.");
	}
	return *(&x + i);
}

inline wm::Vector4f wm::Vector4f::operator*(float n) const
{
	return Vector4f(n * x, n * y, n * z, n * w);
}

inline float wm::Vector4f::operator*(const Vector4f& v) const
{
	return x * v.x + y * v.y + z * v.z + w * v.w;
}
inline wm::Vector4f wm::Vector4f::operator-(const Vector4f& v) const
{
	return Vector4f(x - v.x, y - v.y, z - v.z, w - v.w);
}
inline wm::Vector4f wm::Vector4f::operator-(void) const
{
	return Vector4f(-x, -y, -z, -w);
}
inline wm::Vector4f wm::Vector4f::operator+(const Vector4f& v) const
{
	return Vector4f(x + v.x, y + v.y, z + v.z, w + v.w);
}
inline wm::Vector4f wm::Vector4f::operator+(float n) const
{
	return Vector4f(x + n, y + n, z + n, w + n);
}
inline wm::Vector4f wm::Vector4f::cross_product3(const Vector4f& v) const
{
	return Vector4f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x, 0.f);
}
inline wm::Vector3f wm::Vector4f::vec3() const
{
	if (w == 0) {
		return Vector3f(x, y, z);
	}
	return Vector3f(x / w, y / w, z / w);
}
inline wm::Vector2f wm::Vector4f::vec2() const
{
	if (w == 0) {
		return Vector2f(x, y);
	}
	return Vector2f(x / w, y / w);
}
// !Vector4f class func ---------------------------------------------------------------


// Matrix3f class func ---------------------------------------------------------------
inline wm::Vector3f wm::Matrix3f::operator[](int i) const
{
	if (i < 0 || i >= 3) {
		throw std::runtime_error("index out of bounds in Matrix3f.");
	}
	return matrix[i];
}

inline wm::Vector3f& wm::Matrix3f::operator[](int i)
{
	if (i < 0 || i >= 3) {
		throw std::runtime_error("index out of bounds in Matrix3f.");
	}
	return matrix[i];
}

inline wm::Matrix3f wm::Matrix3f::operator*(float n) const
{
	return Matrix3f(n * matrix[0], n * matrix[1], n * matrix[2]);
}

inline wm::Matrix3f wm::Matrix3f::operator*(const Matrix3f& m) const
{
	Matrix3f res;
	for (int k = 0; k < 3; ++k) {
		for (int i = 0; i < 3; ++i) {
			float val = m[i][k];
			for (int j = 0; j < 3; ++j) {
				res[i][j] += val * m[k][j];
			}
		}
	}
	return res;
}

inline wm::Vector3f wm::Matrix3f::operator*(const Vector3f& v) const
{
	return Vector3f(matrix[0] * v, matrix[1] * v, matrix[2] * v);
}
// !Matrix3f class func ---------------------------------------------------------------


// Matrix4f class func ---------------------------------------------------------------
inline wm::Vector4f wm::Matrix4f::operator[](int i) const
{
	if (i < 0 || i >= 4) {
		throw std::runtime_error("index out of bounds in Matrix4f.");
	}
	return matrix[i];
}

inline wm::Vector4f& wm::Matrix4f::operator[](int i)
{
	if (i < 0 || i >= 4) {
		throw std::runtime_error("index out of bounds in Matrix4f.");
	}
	return matrix[i];
}

inline wm::Matrix4f wm::Matrix4f::operator*(float n) const
{
	return Matrix4f(n * matrix[0], n * matrix[1], n * matrix[2], n * matrix[3]);
}

inline wm::Matrix4f wm::Matrix4f::operator*(const Matrix4f& m) const
{
	Matrix4f res;
	for (int k = 0; k < 4; ++k) {
		for (int i = 0; i < 4; ++i) {
			float val = matrix[i][k];
			for (int j = 0; j < 4; ++j) {
				res[i][j] += val * m[k][j];
			}
		}
	}
	return res;
}

inline wm::Vector4f wm::Matrix4f::operator*(const Vector4f& v) const
{
	return Vector4f(matrix[0] * v, matrix[1] * v, matrix[2] * v, matrix[3] * v);
}

// !Matrix4f class func ---------------------------------------------------------------

// ToDo: transfrom to wm-math.h
namespace wm {
	namespace math {
		inline float fmax3(float x, float y, float z) { return fmax(x, fmax(y, z)); };
		inline float fmin3(float x, float y, float z) { return fmin(x, fmin(y, z)); };

		inline Vector3f blend_color(const Vector3f& a, const Vector3f& b)
		{
			return 1.f - ((1.f - a) + (1.f - b)) * 0.5f;
		}

		inline float interpolate_view_space_z_depth(const Vector3f& barycentric, const Vector3f& z)
		{
			auto [alpha, beta, gamma] = barycentric;
			return -1.f / (alpha / z[0] + beta / z[1] + gamma / z[2]);
		}

		template<class Attr>
		inline Attr interpolate3d(const std::array<Attr, 3>& attr, const Vector3f& barycentric, const std::array<Vector4f, 3>& vex, float z_inter)
		{
			auto [alpha, beta, gamma] = barycentric;
			auto res = z_inter * (
				alpha * attr[0] * (-1.f / vex[0].z) +
				beta * attr[1] * (-1.f / vex[1].z) +
				gamma * attr[2] * (-1.f / vex[2].z)
				);
			return res;
		}

		template<class T>
		inline Vector3f to_vec3f(const T& t)
		{
			return Vector3f(t[0], t[1], t[2]);
		}
	}
}
#endif // !VECTOR__H

