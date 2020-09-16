#ifndef OBJECT__H
#define OBJECT__H
#pragma once


#include <vector>
#include <string>
#include <memory>
#include <array>
#include <algorithm>

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
		Color(const Vector3f& color) :r(std::round(color.x)), g(std::round(color.y)), b(std::round(color.z)) {}
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
		Color operator/(float rhs) const
		{
			return Color(std::round(r / rhs), std::round(g / rhs), std::round(b / rhs));
		}
		Color operator+(const Color& color) const
		{
			return Color(r + color.r, g + color.g, b + color.b);
		}
		Color& operator+=(const Color& color)
		{
			r += color.r, g += color.g, b += color.b;
			return *this;
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

		Vector3f get_color(float u, float v)
		{
			Vector3f res;
			int i = (1.f - v) * (h_ - 1);
			int j = u * (w_ - 1);
			res[0] = 1.f - data_[i * w_ * n_ + j * n_ + 0] / 255.f;
			res[1] = 1.f - data_[i * w_ * n_ + j * n_ + 1] / 255.f;
			res[2] = 1.f - data_[i * w_ * n_ + j * n_ + 2] / 255.f;
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
		virtual Vector3f barycentric2d(const Vector2f& p) const = 0;
	};

	class Triangle :public Object {
	public:
		Triangle(const std::array<Vector4f, 3>& _vex, const std::array<Vector3f, 3>& _color, const std::array<Vector4f, 3>& _n,
			const std::array<float, 3>& _u, const std::array<float, 3>& _v, const std::shared_ptr<Texture>& _texture = nullptr) :
			Object(_vex, _color, _n, _u, _v, _texture) {}
		Vector3f barycentric2d(const Vector2f& p) const
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
			auto ap = Vector2f(p.x - vex[0].x, p.y - vex[0].y);
			auto bp = Vector2f(p.x - vex[1].x, p.y - vex[1].y);
			auto cp = Vector2f(p.x - vex[2].x, p.y - vex[2].y);
			auto ab = Vector2f(vex[1].x - vex[0].x, vex[1].y - vex[0].y);
			auto bc = Vector2f(vex[2].x - vex[1].x, vex[2].y - vex[1].y);
			auto ca = Vector2f(vex[0].x - vex[2].x, vex[0].y - vex[2].y);
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


	class Ray {
		// ToDo
	public:
		Vector4f pos;
		Vector4f direct;
	};

	struct Intersection {
		float t;
		std::shared_ptr<Object> object;

	};

	struct AccelerateNode {
		std::shared_ptr<Object> object;
		std::shared_ptr<AccelerateNode> left;
		std::shared_ptr<AccelerateNode> right;
		Bound bound;
		AccelerateNode() :object(nullptr), left(nullptr), right(nullptr), bound() {}
	};
	class AccelerateStructure {
	protected:
		std::shared_ptr<AccelerateNode> root;
	public:
		virtual Intersection ray_intersect(const Ray& ray) const = 0;
		virtual void build(const std::vector<Mesh>& meshs) const = 0;
	};

	class Bvh :public AccelerateStructure {
	public:
		void build(const std::vector<Mesh>& meshs)
		{
			std::vector<std::shared_ptr<Object> > objects;
			for (const auto& mesh : meshs) {
				for (const auto& object : mesh.objects) {
					objects.push_back(object);
				}
			}
			build_(objects);
		}
	private:
		std::shared_ptr<AccelerateNode> build_(std::vector<std::shared_ptr<Object> >& objects)
		{
			std::shared_ptr<AccelerateNode> node;
			if (objects.size() == 0) {
				node = nullptr;
			}
			else if (objects.size() == 1) {
				node = std::make_shared<AccelerateNode>();
				node->bound = objects[0]->bound;
				node->object = objects[0];
			}
			else {
				float x_max = 0.f, x_min = std::numeric_limits<float>::max();
				float y_max = 0.f, y_min = std::numeric_limits<float>::max();
				float z_max = 0.f, z_min = std::numeric_limits<float>::max();
				for (const auto& object : objects) {
					x_max = std::fmax(x_max, object->bound.upper.x);
					x_min = std::fmin(x_min, object->bound.lower.x);
					y_max = std::fmax(y_max, object->bound.upper.y);
					y_min = std::fmin(y_min, object->bound.lower.y);
					z_max = std::fmax(z_max, object->bound.upper.z);
					z_min = std::fmin(z_min, object->bound.lower.z);
				}
				float x_len = x_max - x_min;
				float y_len = y_max - y_min;
				float z_len = z_max - z_min;

				std::function<bool(const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b)> cmp;
				if (x_len >= y_len && x_len >= z_len) {
					// x len is max, sort by x->y->z
					cmp = [](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
						auto a_center = a->bound.center();
						auto b_center = b->bound.center();
						return a_center.x > b_center.x || a_center.x == b_center.x && a_center.y > b_center.y || a_center.y == b_center.y && a_center.z > b_center.z;
					};
				}
				else if (y_len >= x_len && y_len >= z_len) {
					// y len is max, sort by y->z->x
					cmp = [](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
						auto a_center = a->bound.center();
						auto b_center = b->bound.center();
						return a_center.y > b_center.y || a_center.y == b_center.y && a_center.z > b_center.z || a_center.z == b_center.z && a_center.x > b_center.x;
					};
				}
				else {
					// z len is max, sort by z->x->y
					cmp = [](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
						auto a_center = a->bound.center();
						auto b_center = b->bound.center();
						return a_center.z > b_center.z || a_center.z == b_center.z && a_center.x > b_center.x || a_center.x == b_center.x && a_center.y > b_center.y;
					};
				}
				std::sort(objects.begin(), objects.end(), cmp);

				// ToDo: change BVH to SVH
				auto mid = objects.begin() + objects.size() / 2; // seperate the objects to [begin, mid) and [mid, end)
				auto left_objects = std::vector<std::shared_ptr<Object> >(objects.begin(), mid);
				auto right_objects = std::vector<std::shared_ptr<Object> >(mid, objects.end());

				node->left = build_(left_objects);
				node->right = build_(right_objects);
				node->bound = node->left->bound | node->right->bound;
			}
			return node;
		}
	};

} // !namesapce wm

// global func
inline wm::Color operator*(float rhs, const wm::Color& color)
{
	return color * rhs;
}
#endif // !OBJECT__H
