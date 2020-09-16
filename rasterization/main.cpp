#include "stdafx.h"
#include "rst.h"

#include <memory>
#include <fstream>

using namespace wm;


// ToDo:
// 0. blin-phone light model
// 1. finish bound, realize svh tree
// 2. occlude
// 3. render
// 4. texture
// 5. transform the framework
// 6. ray tracing
// 7. path tracing

int main()
{
	Rst rst(1000, 1000, shader::texture_fragment_shader);
	
	rst.read_obj("./model/spot/spot_triangulated_good.obj", 1.f, { 0,1.f,0,1 });
//	rst.open_msaa(16);

	Camera camera;
	camera.aspect = 1.f;
	camera.fov = 90.f;
	camera.eye_pos = { 0,0,-2,0 };
	camera.look_at = { 0,0,1,0 };
	camera.up = { 0,1,0,0 };
	camera.z_far = 5.f;
	camera.z_near = 0.5f;


	const float pi = 3.1415926;
	auto get_x = [&pi](float theta) {return 2.f * std::cos(theta / 180.f * pi); };
	auto get_z = [&pi](float theta) {return 2.f * std::sin(theta / 180.f * pi); };

	int index = 0;
	
	for (float i = 0.f; i <= 360.f; i += 0.5f) {
		float x = get_x(i);
		float z = get_z(i);
		camera.eye_pos = { x    , 1, z    , 0 };
		camera.look_at = { 0 - x, 0, 0 - z, 0 };
		rst.set_camera(camera);
		std::string filename;
		if (index < 10) {
			filename = "img00" + std::to_string(index++) + ".ppm";
		}
		else if (index < 100) {
			filename = "img0" + std::to_string(index++) + ".ppm";
		}
		else {
			filename = "img" + std::to_string(index++) + ".ppm";
		}
		rst.display_rst(filename);
		std::cout << index << "finished\n";
	}
	return 0;
}