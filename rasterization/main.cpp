#define TINYOBJLOADER_IMPLEMENTATION

#include "stdafx.h"
#include "rst.h"
#include "ObjLoader.h"

#include <memory>
#include <fstream>

using namespace wm;

void read_obj(std::string filename, Rst& rst, float aspect = 1.f, Vector4f pos = { 0,0,0,1 })
{
	std::string err;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	bool success = tinyobj::LoadObj(shapes, materials, err, filename.c_str());

	std::array<Vector3f, 3> color;
	color[0] = color[1] = color[2] = { 255,255,255 };
	std::array<Vector3f, 3> normal;
	std::array<Vector4f, 3> coor;
	std::array<float, 3> u;
	std::array<float, 3> v;

	if (success) {
		for (const auto& shape : shapes) {
			Mesh mesh;

			int index = 0;
			for (int i = 0; i < shape.mesh.num_vertices.size(); ++i) {
				int num = shape.mesh.num_vertices[i]; // for triangle, the value is 3
				for (int j = 0; j < num; ++j) {
					// for each vex
					int id = shape.mesh.indices[index++];

					for (int k = 0; k < 3; ++k) {
						// read coor & normals
						coor[j][k] = shape.mesh.positions[3 * id + k];

						if (shape.mesh.normals.size() > 0) {
							normal[j][k] = shape.mesh.normals[3 * id + k];
						}
					}
					coor[j][3] = 1.f;

					// for u, v
					if (shape.mesh.texcoords.size() > 0) {
						u[j] = shape.mesh.texcoords[2 * id + 1];
						v[j] = shape.mesh.texcoords[2 * id + 2];
					}
				}
				mesh.objects.emplace_back(std::make_shared<Triangle>(coor, color, normal, u, v));
			}
			// ToDo: <bug> why the pos can change the rotate direct??? 2020-8-29
			//     : maybe my eye is blind. 2020-8-30
			rst.add_mesh(std::move(mesh), aspect, pos);
		}
	}
	else {
		std::cout << err << "\n";
		std::cout << "open failed.\n";
	}
}

// ToDo:
// 1. finish bound, realize svh tree
// 2. occlude
// 3. render
// 4. texture
// 5. transform the framework
// 6. ray tracing
// 7. path tracing

int main()
{
	//std::array<Vector4f, 3> arr; 
	//arr[0] = { 1, 1, 0, 1 };
	//arr[1] = { 0, 0, 0, 1 };
	//arr[2] = { 1, 0, 0, 1 };
	//Triangle t(arr, {}, {});
	//auto [a, b, c] = t.barycentric2d(Vector2f(1, 0));
	//std::cout << a << " " << b << " " << c << std::endl;

	Rst rst(1000, 1000);
	
	read_obj("./model/97-free_091_aya_obj/091_W_Aya_100K.obj", rst, 0.002f, { 0,0,0,1 });
	rst.show_objects();

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