#ifndef OBJECT__H
#define OBJECT__H
#pragma once


#include <vector>
#include <string>
#include <memory>
#include <array>
#include <tuple>

#include "Vector.h"
#include "Bound.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

namespace wm
{
	struct Color {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		// int operator()() { return RGB(r, g, b); } // need Windows.h
		Color() :r(0), g(0), b(0) {}
		Color(unsigned char _r, unsigned char _g, unsigned char _b)
			:r(_r), g(_g), b(_b) {}
		Color(unsigned char color) :r(color), g(color), b(color) {}
		Color(const Color& color) :r(color.r), g(color.g), b(color.b) {}
		unsigned char& operator[](int i)
		{
			return *(&r + i);
		}
		unsigned char operator[](int i) const
		{
			return *(&r + i);
		}
		Color operator*(float rhs) const
		{
			return Color(r * rhs, g * rhs, b * rhs);
		}
		Color operator+(Color color) const
		{
			return Color(r + color.r, g + color.g, b + color.b);
		}
	};

	class Texture {
	private:
		unsigned char* data_;
		int w_;
		int h_;
		int n_;
	public:
		Texture(std::string filename)
			:data_(nullptr), w_(0), h_(0), n_(0)
		{
			//data_ = stbi_load(filename.c_str(), &w_, &h_, &n_, 0);
		}
		Texture(unsigned char* data, int w, int h, int n)
			:data_(data), w_(w), h_(h), n_(n)
		{}

		Color get_color(float u, float v)
		{
			Color res;
			int i = (1.f - v) * (h_ - 1);
			int j = u * (w_ - 1);
			res.r = data_[i * w_ * n_ + j * n_ + 0];
			res.g = data_[i * w_ * n_ + j * n_ + 1];
			res.b = data_[i * w_ * n_ + j * n_ + 2];
			return res;
		}
		~Texture() { delete data_; }
	};

	class Object {
	public:
		std::array<Vector4f, 3> vex;
		std::array<Vector3f, 3> color;
		std::array<Vector4f, 3> n;
		std::array<float, 3>    u;
		std::array<float, 3>    v;
		std::shared_ptr<Texture> texture;
		Bound                   bound;

		Object(const std::array<Vector4f, 3>& _vex) :vex(_vex), color(), n(), u(), v(), texture(nullptr), bound() {} // not implement, for test now
		Object(const std::array<Vector4f, 3>& _vex, const std::array<Vector3f, 3>& _color, const std::array<Vector4f, 3>& _n,
			const std::array<float, 3> _u, const std::array<float, 3> _v, const std::shared_ptr<Texture>& _texture=nullptr) :
			vex(_vex), color(_color), n(_n), u(_u), v(_v), texture(_texture)
		{
			using std::fmax;
			Vector3f lower = {
				fmin(fmin(vex[0][0], vex[1][0]), vex[2][0]),
				fmin(fmin(vex[0][1], vex[1][1]), vex[2][1]),
				fmin(fmin(vex[0][2], vex[1][2]), vex[2][2])
			};

			Vector3f upper = {
				fmax(fmax(vex[0][0], vex[1][0]), vex[2][0]),
				fmax(fmax(vex[0][1], vex[1][1]), vex[2][1]),
				fmax(fmax(vex[0][2], vex[1][2]), vex[2][2])
			};
			bound = Bound(lower, upper);
		}

		virtual std::shared_ptr<Object> to_clip_space(const Matrix4f& vp) const = 0;
		virtual bool is_contain_point2d(const Vector2f& p) const = 0;
		virtual Vector3f barycentric2d(const Vector2f& p) = 0;
	};

	class Triangle :public Object {
	public:
		Triangle(const std::array<Vector4f, 3>& _vex, const std::array<Vector3f, 3>& _color, const std::array<Vector4f, 3>& _n,
			const std::array<float, 3>& _u, const std::array<float, 3>& _v, const std::shared_ptr<Texture>& _texture = nullptr) :
			Object(_vex, _color, _n, _u, _v, _texture) {}
		Vector3f barycentric2d(const Vector2f& p)
		{
			auto ab = vex[1] - vex[0];
			auto ac = vex[2] - vex[0];
			auto pa = vex[0] - Vector4f(p.x, p.y, 0.f, 1.f); // z, w if only for padding
			
			Vector3f vx(ab.x, ac.x, pa.x);
			Vector3f vy(ab.y, ac.y, pa.y);

			auto u = vx.cross_product(vy);
			return Vector3f(1.f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
		}
		bool is_contain_point2d(const Vector2f& p) const
		{
			auto ap = p - vex[0].vec2();
			auto bp = p - vex[1].vec2();
			auto cp = p - vex[2].vec2();
			auto ab = vex[1].vec2() - vex[0].vec2();
			auto bc = vex[2].vec2() - vex[1].vec2();
			auto ca = vex[0].vec2() - vex[2].vec2();
			return ab.cross_product(ap).z >= 0 && bc.cross_product(bp).z >= 0 && ca.cross_product(cp).z >= 0;
		}
		std::shared_ptr<Object> to_clip_space(const Matrix4f& vp) const
		{
			auto vec_arr = std::array<Vector4f, 3>({ vp * vex[0], vp * vex[1], vp * vex[2] });
			for (auto& vec : vec_arr) {
				vec.standard();
			}
			return std::make_shared<Triangle>(vec_arr, color, n, u, v, texture);
		}
	};

	class Mesh {
	public:
		std::vector<std::shared_ptr<Object> > objects;
		Vector4f pos;
		Vector4f up;
		Vector4f front;

	public:
		Mesh() :objects(), pos(0, 0, 0, 1), up(0, 1, 0, 0) {}
		Mesh(
			const std::vector<std::shared_ptr<Object> >& _objects,
			Vector4f _pos = { 0,0,0,1 },
			Vector4f _up = { 0,1,0,0 },
			Vector4f _front = {0,0,1,0}
		) :objects(_objects), pos(_pos), up(_up) {}
		Mesh(
			std::vector<std::shared_ptr<Object> >&& _objects,
			Vector4f _pos = { 0,0,0,1 },
			Vector4f _up = { 0,1,0,0 },
			Vector4f _front = { 0,0,1,0 }
		) :objects(_objects), pos(_pos), up(_up) {}

		Mesh(const Mesh& mesh) {
			objects = mesh.objects;
			pos = mesh.pos;
			up = mesh.up;
			front = mesh.front;
		}
		Mesh(Mesh&& mesh) noexcept {
			objects = std::move(mesh.objects);
			pos = mesh.pos;
			up = mesh.up;
			front = mesh.front;
		}

		void to_world(const Matrix4f& matrix) {
			// error, the other attr not changed.

			// change triangle's coor
			for (auto& object : objects) {
				for (int i = 0; i < 3; ++i) {
					object->vex[i] = matrix * object->vex[i];
					object->vex[i].standard();
				}
			}

			//change pos
			pos = matrix * pos;
			pos.standard();

			// change up
			up = matrix[2];
			
			// change front
			front = matrix[3];
		}
	};

} // !namesapce wm

// global func
inline wm::Color operator*(float rhs, const wm::Color& color)
{
	return color * rhs;
}
#endif // !OBJECT__H
