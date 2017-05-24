#include "mesh_gen.hpp"
#include "bezier.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace kvant {
namespace graphics {

    Triangle_mesh<> generate_smooth_cube(const glm::vec3& size,
                                               unsigned level_of_detail)
    {
        const float cube_size = 1.0f;
        const float fluff = 0.05f;
        const float corner_scale = 0.92f;
        const float inner_scale = 1.0f;

        Bezier_patch<glm::vec3, float> p0({glm::vec3(-cube_size, cube_size, cube_size) * corner_scale,
                                                      glm::vec3(-(cube_size - fluff), cube_size, cube_size),
                                                      glm::vec3((cube_size - fluff), cube_size, cube_size),
                                                      glm::vec3(cube_size, cube_size, cube_size) * corner_scale,
                                                      glm::vec3(-cube_size, (cube_size - fluff), cube_size),
                                                      glm::vec3(-(cube_size - fluff), (cube_size - fluff), cube_size + fluff) * inner_scale,
                                                      glm::vec3((cube_size - fluff), (cube_size - fluff), cube_size + fluff) * inner_scale,
                                                      glm::vec3(cube_size, (cube_size - fluff), cube_size),
                                                      glm::vec3(-cube_size, -(cube_size - fluff), cube_size),
                                                      glm::vec3(-(cube_size - fluff), -(cube_size - fluff), cube_size + fluff) * inner_scale,
                                                      glm::vec3((cube_size - fluff), -(cube_size - fluff), cube_size + fluff) * inner_scale,
                                                      glm::vec3(cube_size, -(cube_size - fluff), cube_size),
                                                      glm::vec3(-cube_size, -cube_size, cube_size) * corner_scale,
                                                      glm::vec3(-(cube_size - fluff), -cube_size, cube_size),
                                                      glm::vec3((cube_size - fluff), -cube_size, cube_size),
                                                      glm::vec3(cube_size, -cube_size, cube_size) * corner_scale});

        Triangle_mesh<> patch;
        patch.make_patch(p0, level_of_detail, level_of_detail);

        Triangle_mesh<> cube;
        cube.merge(patch);
        patch.transform(glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
        cube.merge(patch);
        patch.transform(glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
        cube.merge(patch);
        patch.transform(glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f)));
        cube.merge(patch);
        patch.transform(glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
        cube.merge(patch);
        patch.transform(glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)));
        cube.merge(patch);

        cube.optimize();
        //cube.make_non_indexed();
        cube.calculate_vertex_normals();
        cube.foreach_vertex([](Vertex& v) { v.color = glm::vec3(1.0f, 0.0f, 0.0f); });
        cube.scale(size);

        return cube;
    }

    Triangle_mesh<> generate_cube(const glm::vec3& size)
    {
        const float top = size.y;
        const float bottom = -top;
        const float right = size.x;
        const float left = -right;
        const float far = -size.z;
        const float near = -far;

        Triangle_mesh<> cube;
        { // top
            std::vector<glm::vec3> quad{{left, top, far},
                                        {right, top, far},
                                        {right, top, near},
                                        {right, top, near},
                                        {left, top, near},
                                        {left, top, far}};

            cube.merge_triangles(quad);
        }
        { // bottom
            std::vector<glm::vec3> quad{{left, bottom, -far},
                                        {right, bottom, -far},
                                        {right, bottom, -near},
                                        {right, bottom, -near},
                                        {left, bottom, -near},
                                        {left, bottom, -far}};

            cube.merge_triangles(quad);
        }
        { // right
            std::vector<glm::vec3> quad{{right, top, near},
                                        {right, top, far},
                                        {right, bottom, far},
                                        {right, bottom, far},
                                        {right, bottom, near},
                                        {right, top, near}};

            cube.merge_triangles(quad);
        }
        { // left
            std::vector<glm::vec3> quad{{left, top, -near},
                                        {left, top, -far},
                                        {left, bottom, -far},
                                        {left, bottom, -far},
                                        {left, bottom, -near},
                                        {left, top, -near}};

            cube.merge_triangles(quad);
        }
        { // far
            std::vector<glm::vec3> quad{{right, top, far},
                                        {left, top, far},
                                        {left, bottom, far},
                                        {left, bottom, far},
                                        {right, bottom, far},
                                        {right, top, far}};

            cube.merge_triangles(quad);
        }
        { // near
            std::vector<glm::vec3> quad{{-right, top, near},
                                        {-left, top, near},
                                        {-left, bottom, near},
                                        {-left, bottom, near},
                                        {-right, bottom, near},
                                        {-right, top, near}};

            cube.merge_triangles(quad);
        }

        cube.calculate_vertex_normals();
        cube.foreach_vertex([](Vertex& v) { v.color = glm::vec3(1.0f, 0.0f, 0.0f); });

        return cube;
    }

    Triangle_mesh<> generate_checkerboard(unsigned int width,
                                        unsigned int height,
                                        float unit_size)
    {
		const float start_x = -static_cast<float>(width)*0.5f*unit_size;
		const float start_z = -static_cast<float>(height)*0.5f*unit_size;

		const float step_x = unit_size;
		const float step_z = unit_size;

		std::vector<glm::vec3> vertices;

		float z = start_z;
		for (unsigned counter_z = 0; counter_z < height; ++counter_z)
		{
			float x = start_x;

			for (unsigned counter_x = 0; counter_x < width; ++counter_x)
			{
				vertices.push_back({x, 0.0f, z});
				vertices.push_back({x + step_x, 0.0f, z});
				vertices.push_back({x + step_x, 0.0f, z + step_z});

				vertices.push_back({x + step_x, 0.0f, z + step_z});
				vertices.push_back({x, 0.0f, z + step_z});
				vertices.push_back({x, 0.0f, z});

				x += step_x;
			}

			z += step_z;
		}

        Triangle_mesh<> grid;
		grid.merge_triangles(vertices);
		grid.calculate_vertex_normals();
		//grid.foreach_vertex([](graphics::mesh::Vertex& v) { v.normal = glm::vec3(0.0f, 1.0f, 0.0f); });

		for (size_t i = 0; i < grid.vertices.size(); i += 6)
		{
			static const glm::vec3 white{ 1.0f };
			static const glm::vec3 grey{ 1.0f, 0.0f, 0.0f };

			const glm::vec3 color = ((i / 6 + (i/6/width)%2) % 2) == 0 ? white : grey;
			grid.vertices[i + 0].color = color;
			grid.vertices[i + 1].color = color;
			grid.vertices[i + 2].color = color;
			grid.vertices[i + 3].color = color;
			grid.vertices[i + 4].color = color;
			grid.vertices[i + 5].color = color;
		}

        return grid;
    }

} // namespace shape_gen
} // namespace graphics
