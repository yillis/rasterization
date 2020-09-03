#include "stdafx.h"
#include "rst.h"

const float PI = 3.1415926;


void wm::Rst::add_mesh(const Mesh& mesh, float aspect, Vector4f pos)
{
	generate_model(pos, aspect);
	Mesh temp = mesh;
	temp.to_world(model_);
	meshs_.push_back(std::move(temp));
}

void wm::Rst::add_mesh(Mesh&& mesh, float aspect, Vector4f pos)
{
	generate_model(pos, aspect);
	mesh.to_world(model_);
	meshs_.push_back(mesh);
}

void wm::Rst::display_line(const std::string& filename)
{
	for (auto& v : buffer_) {
		std::fill(v.begin(), v.end(), Color(0, 0, 0));
	}
	
	auto vp = projection_ * view_;
	for (const auto& mesh : meshs_) {
		for (const auto& object : mesh.objects) {
			auto clip_object = object->to_clip_space(vp);
			auto [a, b, c] = clip_object->vex;
			draw_rectangle(a, b, c);
		}
	}
	save_file(filename);
}

void wm::Rst::display_rst(const std::string& filename)
{
	for (auto& v : depth_) {
		std::fill(v.begin(), v.end(), (std::numeric_limits<float>::max)());
	}
	for (auto& v : buffer_) {
		std::fill(v.begin(), v.end(), Color(0, 0, 0));
	}
	auto vp = projection_ * view_;
	for (const auto& mesh : meshs_) {
		for (const auto& object : mesh.objects) {
			auto clip_object = object->to_clip_space(vp);
			// ToDo: write to_view_space & to_screen_space
			if (is_inside_scene(clip_object->vex[0]) || is_inside_scene(clip_object->vex[1]) || is_inside_scene(clip_object->vex[2])) {
				for (int i = 0; i < 3; ++i) {
					clip_object->vex[i][0] = get_width(clip_object->vex[i][0]);
					clip_object->vex[i][1] = get_height(clip_object->vex[i][1]);
					clip_object->vex[i][2] = (view_[2] * object->vex[i]) / (view_[3] * object->vex[i]);
				}
				rasterization(object, clip_object);
			}
		}
	}
	save_file(filename);
}

void wm::Rst::plot(int x, int y, const Color& color)
{
	x = scene_height_ - 1 - x;
	if (x >= 0 && x < scene_height_ && y >= 0 && y < scene_width_) {
		buffer_[x][y] = color;
	}
}

// clip_object->v: 0(screen space coor x), 1(screen space coor y), 2(view space coor z)
void wm::Rst::rasterization(const std::shared_ptr<Object>& object, const std::shared_ptr<Object>& clip_object)
{
	// ToDo: msaa

	// the index is very confuse, need to transform
	using std::fmin;
	using std::fmax;
	auto fmax3 = [](float x, float y, float z) {return fmax(x, fmax(y, z)); };
	auto fmin3 = [](float x, float y, float z) {return fmin(x, fmin(y, z)); };

	// *must limit the x, y to screen, otherwise, the x, y out of screen will be calculate, it's a big time-cost
	int x_min = std::round(fmax(0.f          , fmin3(clip_object->vex[0].x, clip_object->vex[1].x, clip_object->vex[2].x)));
	int x_max = std::round(fmin(scene_width_ , fmax3(clip_object->vex[0].x, clip_object->vex[1].x, clip_object->vex[2].x)));
	int y_min = std::round(fmax(0.f          , fmin3(clip_object->vex[0].y, clip_object->vex[1].y, clip_object->vex[2].y)));
	int y_max = std::round(fmin(scene_height_, fmax3(clip_object->vex[0].y, clip_object->vex[1].y, clip_object->vex[2].y)));

	// <bug>, the pos is not right, think a good way to deal. 2020-9-1
	// fixed, modify the triangle's pos to screen space. 2020-9-1

	// <bug>, the z-buffer alogorithm is not right. 2020-9-2
	// fixed!!! in view Matrix, the coor z is rotate to -z axis, so the z_inter must mul -1.f!!! 2020-9-2
	for (int x = x_min; x < x_max; ++x) {
		for (int y = y_min; y < y_max; ++y) {
			auto p = Vector2f(x + 0.5f, y + 0.5f);
			if (clip_object->is_contain_point2d(p)) {
				//      a      b     c
				auto [alpha, beta, gamma] = clip_object->barycentric2d(p);
				float z_inter = -1.f / (alpha / clip_object->vex[0].z + beta / clip_object->vex[1].z + gamma / clip_object->vex[2].z);
				auto normal = z_inter * (
					alpha * object->n[0] * (1.f / clip_object->vex[0].z) +
					beta * object->n[1] * (1.f / clip_object->vex[1].z) +
					gamma * object->n[2] * (1.f / clip_object->vex[2].z)
					);
				normal = (normal + Vector3f(1.f, 1.f, 1.f)) * 0.5f;
				if (x >= 0 && x < scene_width_ && y >= 0 && y < scene_height_) {
					if (z_inter < depth_[scene_height_ - 1 - y][x]) {
						// ToDo: use payload shader
						plot(y, x, Color(255 * normal[0], 255 * normal[1], 255 * normal[2]));
						depth_[scene_height_ - 1 - y][x] = z_inter;
					}
				}
			}
		}
	}
}

void wm::Rst::generate_model(const Vector4f& center_pos, float aspect)
{
	Matrix4f scale = {
		{aspect, 0     , 0     , 0},
		{0     , aspect, 0     , 0},
		{0     , 0     , aspect, 0},
		{0     , 0     , 0     , 1}
	};

	// ToDo
	// Matrix4f rotate = identity<Matrix4f, 4>();

	Matrix4f move = {
		{1, 0, 0, center_pos[0]},
		{0, 1, 0, center_pos[1]},
		{0, 0, 1, center_pos[2]},
		{0, 0, 0,             1}
	};
	model_ = move * scale;
}

void wm::Rst::generate_view()
{
	Matrix4f move(
		{ 1,0,0,-camera_.eye_pos[0] },
		{ 0,1,0,-camera_.eye_pos[1] },
		{ 0,0,1,-camera_.eye_pos[2] },
		{ 0,0,0,                  1 }
	);

	Matrix4f rotate(
		normalize(camera_.look_at.cross_product3(camera_.up)),
		normalize(camera_.up),
		normalize(-camera_.look_at),
		{ 0, 0, 0, 1 }
	);
	view_ = rotate * move;
}

void wm::Rst::generate_projection()
{
	Matrix4f perspective(
		{ -camera_.z_near,              0,                              0,                               0 },
		{ 0,              -camera_.z_near,                              0,                               0 },
		{ 0,                           0, -camera_.z_near - camera_.z_far, -camera_.z_near * camera_.z_far },
		{ 0,                           0,                              1,                               0 }
	);

	Matrix4f move(
		{ 1, 0, 0,                                     0 },
		{ 0, 1, 0,                                     0 },
		{ 0, 0, 1,  (camera_.z_far + camera_.z_near) / 2 },
		{ 0, 0, 0,                                     1 }
	);

	float width = camera_.z_near * std::tan(camera_.fov / 2.f / 180 * PI);
	float length = width * camera_.aspect;
	Matrix4f scale(
		{ 1.f / length,           0,                                    0, 0 },
		{ 0,            1.f / width,                                    0, 0 },
		{ 0,                      0, 2 / (camera_.z_far - camera_.z_near), 0 },
		{ 0,                      0,                                    0, 1 }
	);
	projection_ = scale * move * perspective;
}

void wm::Rst::draw_line(float x0, float y0, float x1, float y1)
{
	// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm

	auto ipart = [](float x) {return std::floor(x); };
	auto fpart = [](float x) {return x - std::floor(x); };
	auto rfpart = [&fpart](float x) {return 1.f - fpart(x); };

	bool steep = fabs(y1 - y0) > fabs(x1 - x0);

	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	float dx = x1 - x0;
	float dy = y1 - y0;
	float gradient;
	if (dx == 0) {
		gradient = 1.f;
	}
	else {
		gradient = dy / dx;
	}

	float x_end = std::round(x0);
	float y_end = y0 + gradient * (x_end - x0);
	float x_gap = rfpart(x0 + 0.5f);
	int xpxl1 = x_end;
	int ypxl1 = ipart(y_end);
	if (steep) {
		plot(ypxl1, xpxl1, rfpart(y_end) * x_gap * 255.f);
		plot(ypxl1 + 1, xpxl1, fpart(y_end) * x_gap * 255.f);
	}
	else {
		plot(xpxl1, ypxl1, rfpart(y_end) * x_gap * 255.f);
		plot(xpxl1, ypxl1 + 1, fpart(y_end) * x_gap * 255.f);
	}
	float intery = y_end + gradient;

	x_end = std::round(x1);
	y_end = y1 + gradient * (x_end - x1);
	x_gap = fpart(x1 + 0.5f);
	int xpxl2 = x_end;
	int ypxl2 = ipart(y_end);

	if (steep) {
		plot(ypxl2, xpxl2, rfpart(y_end) * x_gap * 255.f);
		plot(ypxl2 + 1, xpxl2, fpart(y_end) * x_gap * 255.f);
	}
	else {
		plot(xpxl2, ypxl2, rfpart(y_end) * x_gap * 255.f);
		plot(xpxl2, ypxl2 + 1, fpart(y_end) * x_gap * 255.f);
	}

	if (steep) {
		for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x) {
			plot(ipart(intery)      , x, rfpart(intery) * 255.f);
			plot(ipart(intery) + 1, x,  fpart(intery) * 255.f);
			intery = intery + gradient;
		}
	}
	else {
		for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x) {
			plot(x, ipart(intery)    , rfpart(intery) * 255.f);
			plot(x, ipart(intery) + 1,  fpart(intery) * 255.f);
			intery = intery + gradient;
		}
	}
}

bool wm::Rst::is_inside_scene(const Vector4f& v)
{
	return v.y < 1.f && v[0] >= -1.f && v[1] < 1.f && v[1] >= -1.f && v[2] < 1.f && v[2] >= -1.f;
}

void wm::Rst::save_file(const std::string& filename)
{
	std::ofstream file("./img/" + filename, std::ios::out | std::ios::binary);
	file << "P6\n" << scene_width_ << " " << scene_height_ << "\n" << "255\n";
	for (int i = 0; i < scene_height_; ++i) {
		for (int j = 0; j < scene_width_; ++j) {
			file << buffer_[i][j].r << buffer_[i][j].g << buffer_[i][j].b;
		}
	}
}

void wm::Rst::draw_rectangle(const Vector4f& a, const Vector4f& b, const Vector4f& c)
{
	// ToDo: <bug> the right height scene's line is not right. 2020-8-29
	// ¡Ì   : worked, the index calculated error. 2020-8-30
	using std::round;

	if (is_inside_scene(a) || is_inside_scene(b)) {
		draw_line(round(get_height(a.y)), round(get_width(a.x)), round(get_height(b.y)), round(get_width(b.x)));
	}
	if (is_inside_scene(b) || is_inside_scene(c)) {
		draw_line(round(get_height(b.y)), round(get_width(b.x)), round(get_height(c.y)), round(get_width(c.x)));
	}
	if (is_inside_scene(c) || is_inside_scene(a)) {
		draw_line(round(get_height(c.y)), round(get_width(c.x)), round(get_height(a.y)), round(get_width(a.x)));
	}
}
