#include "shape_gen.hpp"
#include "bezier.hpp"
#include <glm/glm.hpp>

namespace graphics { 
namespace shape_gen {

	Triangle_mesh<> smooth_cube(const glm::vec3& size, unsigned level_of_detail)
	{
		const float cube_size = 0.8f;
		const float fluff = 0.2f;
		const float corner_scale = 0.9;
		const float inner_scale = 0.8f;

		graphics::bezier::Patch<glm::vec3, float> p0({
			glm::vec3(-cube_size, cube_size, cube_size)*corner_scale,		glm::vec3(-(cube_size - fluff), cube_size, cube_size),									glm::vec3((cube_size - fluff), cube_size, cube_size),									glm::vec3(cube_size, cube_size, cube_size)*corner_scale, 
			glm::vec3(-cube_size, (cube_size - fluff), cube_size),			glm::vec3(-(cube_size - fluff), (cube_size - fluff), cube_size + fluff)*inner_scale,	glm::vec3((cube_size - fluff), (cube_size - fluff), cube_size + fluff)*inner_scale,		glm::vec3(cube_size, (cube_size - fluff), cube_size), 
			glm::vec3(-cube_size, -(cube_size - fluff), cube_size),			glm::vec3(-(cube_size - fluff), -(cube_size - fluff), cube_size + fluff)*inner_scale,	glm::vec3((cube_size - fluff), -(cube_size - fluff), cube_size + fluff)*inner_scale,	glm::vec3(cube_size, -(cube_size - fluff), cube_size), 
			glm::vec3(-cube_size, -cube_size, cube_size)*corner_scale,		glm::vec3(-(cube_size - fluff), -cube_size, cube_size),									glm::vec3((cube_size - fluff), -cube_size, cube_size),									glm::vec3(cube_size, -cube_size, cube_size)*corner_scale
		});

		graphics::mesh::Triangle_mesh<> patch;
		patch.make_patch(p0, 4, 4);

		graphics::mesh::Triangle_mesh<> cube;
		cube.merge(patch);
		patch.transform(glm::rotate(glm::pi<float>()*0.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
		cube.merge(patch);
		patch.transform(glm::rotate(glm::pi<float>()*0.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
		cube.merge(patch);
		patch.transform(glm::rotate(glm::pi<float>()*0.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
		cube.merge(patch);
		patch.transform(glm::rotate(glm::pi<float>()*0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
		cube.merge(patch);
		patch.transform(glm::rotate(glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)));
		cube.merge(patch);

		cube.optimize();
		//cube.make_non_indexed();
		cube.calculate_vertex_normals();
		cube.foreach_vertex([](graphics::mesh::Vertex& v) { v.color = glm::vec3(1.0f, 0.0f, 0.0f); });
		cube.scale(glm::vec3(5.0f));
	}

}}
