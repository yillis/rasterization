#ifndef RST__H
#define RST__H
#pragma once

#include <vector>
#include <string>
#include <limits>
#include <memory>
#include <fstream>
#include <iostream>
#include <functional>

#include "Object.h"
#include "Camera.h"
#include "Shader.h"

namespace wm
{

	class Rst {
	private:
		std::vector<Mesh> meshs_;
		Matrix4f model_;
		Matrix4f view_;
		Matrix4f projection_;
		Camera camera_;
		size_t scene_height_;
		size_t scene_width_;
		std::function<Color(const Payload&)> shader_;
	public:
		Rst(size_t scene_height, size_t scene_width, const std::function<Color(const Payload&)> shader=normal_shader)
			:
			meshs_(),
			model_(),
			view_(),
			projection_(),
			camera_(),
			scene_height_(scene_height),
			scene_width_(scene_width),
			shader_(shader),
			buffer_(std::vector<std::vector<Color> >(scene_height, std::vector<Color>(scene_width))),
			depth_(std::vector<std::vector<float> >(scene_height, std::vector<float>(scene_width, (std::numeric_limits<float>::max)())))
		{}
		// ToDo: rotate
		void add_mesh(const Mesh& mesh, float aspect = 1.f, Vector4f pos = { 0,0,0,1 });
		void add_mesh(Mesh&& mesh, float aspect = 1.f, Vector4f pos = { 0,0,0,1 });
		
		void set_camera(const Camera& camera) { camera_ = camera; generate_view(); generate_projection(); }
		void show_objects()
		{

			float max_x = 0.f, max_y = 0.f, max_z = 0.f;
			float min_x = 100.f, min_y = 100.f, min_z = 100.f;
			for (auto& mesh : meshs_) {
				for (auto& object : mesh.objects) {
					for (int i = 0; i < 3; ++i) {
						max_x = std::fmax(max_x, object->vex[i][0]);
						max_y = std::fmax(max_y, object->vex[i][1]);
						max_z = std::fmax(max_z, object->vex[i][2]);
						min_x = std::fmin(min_x, object->vex[i][0]);
						min_y = std::fmin(min_y, object->vex[i][1]);
						min_z = std::fmin(min_z, object->vex[i][2]);
						// std::cout << object->v[i][0] << " " << object->v[i][1] << " " << object->v[i][2] << " " << object->v[i][3] << std::endl;
					}
				}
				std::cout << max_x << " " << max_y << " " << max_z << "\n";
				std::cout << min_x << " " << min_y << " " << min_z << "\n";
				std::cout << (max_x + min_x) / 2 << " " << (max_y + min_y) / 2 << " " << (max_z + min_z) / 2 << "\n";
			}
		}
		void display_line(const std::string& filename = "temp.ppm");
		void display_rst(const std::string& filename = "temp.ppm");
	private:
		std::vector<std::vector<Color> > buffer_;
		std::vector<std::vector<float> > depth_;

		void plot(int x, int y, const Color& color);
		void rasterization(const std::shared_ptr<Object>& object, const std::shared_ptr<Object>& clip_object);
		void generate_model(const Vector4f& center_pos, float aspect);
		void generate_view();
		void generate_projection();
		void draw_line(float start_x, float start_y, float end_x, float end_y);
		bool is_inside_scene(const Vector4f& v);
		void save_file(const std::string& filename);
		void draw_rectangle(const Vector4f& a, const Vector4f& b, const Vector4f& c);
		int get_height(float x) { return x / 2.f * scene_height_ + scene_height_ / 2.f; }
		int get_width(float y) { return y / 2.f * scene_width_ + scene_width_ / 2.f; }
		float get_height_inv(int x) { return (x - scene_height_ / 2.f) * 2.f / scene_height_; }
		float get_width_inv(int y) { return (y - scene_width_ / 2.f) * 2.f / scene_width_; }
	};

}
#endif // !RST__H
