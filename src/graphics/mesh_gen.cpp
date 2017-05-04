#include "mesh_gen.hpp"
#include <cassert>

namespace graphics {
namespace mesh { 

		Triangle_array triangulate_shape(	unsigned num_segments, 
											unsigned segment_size)
		{
			assert(num_segments > 1);
			assert(segment_size > 1);

			Triangle_array triangles;
			triangles.reserve((num_segments - 1)*segment_size * 2);
			for (	unsigned segment = 0;
					segment < (num_segments - 1);
					++segment)
			{
				for (	unsigned i = 0;
						i < segment_size;
						++i)
				{ 
					const unsigned offset = segment_size*segment;
					auto wrap = [segment_size](unsigned i){ return i%segment_size; };

					{
						unsigned v0 = i + offset;
						unsigned v1 = wrap(i + 1) + offset;
						unsigned v2 = wrap(i + 1) + segment_size + offset;
						triangles.emplace_back(v0, v1, v2);
					} 

					{
						unsigned v0 = wrap(i + 1) + segment_size + offset;
						unsigned v1 = i + segment_size + offset;
						unsigned v2 = i + offset;
						triangles.emplace_back(v0, v1, v2);
					}
				} 
			} 

			return triangles;
		}

		//
		Triangle_mesh<> generate_cube(const glm::vec3& size, const glm::vec3& color)
		{
			const unsigned num_vertices_cube = 4*2;

			Triangle_mesh<> cube;
			cube.vertices.reserve(num_vertices_cube);

			const float s = 0.5f;

			const float left = -s;
			const float right = -left;

			const float top = s;
			const float bottom = -top;

			const float near = s;
			const float far = -near;

			auto emit_indices = [](Triangle_array& triangles, unsigned i)
			{
				triangles.push_back(Triangle(i + 0, i + 1, i + 2));
				triangles.push_back(Triangle(i + 2, i + 3, i + 0));
			};

			// Top
			cube.vertices.emplace_back(glm::vec3(left, top, far)); 
			cube.vertices.emplace_back(glm::vec3(right, top, far)); 
			cube.vertices.emplace_back(glm::vec3(right, top, near)); 
			cube.vertices.emplace_back(glm::vec3(left, top, near)); 
			emit_indices(cube.triangles, cube.vertices.size() - 4);

			// Bottom
			cube.vertices.emplace_back(glm::vec3(left, bottom, near));
			cube.vertices.emplace_back(glm::vec3(right, bottom, near));
			cube.vertices.emplace_back(glm::vec3(right, bottom, far));
			cube.vertices.emplace_back(glm::vec3(left, bottom, far));
			emit_indices(cube.triangles, cube.vertices.size() - 4);

			// Right
			cube.vertices.emplace_back(glm::vec3(right, top, near));
			cube.vertices.emplace_back(glm::vec3(right, top, far));
			cube.vertices.emplace_back(glm::vec3(right, bottom, far));
			cube.vertices.emplace_back(glm::vec3(right, bottom, near));
			emit_indices(cube.triangles, cube.vertices.size() - 4);

			// Left
			cube.vertices.emplace_back(glm::vec3(left, top, far));
			cube.vertices.emplace_back(glm::vec3(left, top, near));
			cube.vertices.emplace_back(glm::vec3(left, bottom, near));
			cube.vertices.emplace_back(glm::vec3(left, bottom, far));
			emit_indices(cube.triangles, cube.vertices.size() - 4);

			// Near
			cube.vertices.emplace_back(glm::vec3(left, top, near));
			cube.vertices.emplace_back(glm::vec3(right, top, near));
			cube.vertices.emplace_back(glm::vec3(right, bottom, near));
			cube.vertices.emplace_back(glm::vec3(left, bottom, near));
			emit_indices(cube.triangles, cube.vertices.size() - 4);

			// Far
			cube.vertices.emplace_back(glm::vec3(right, top, far));
			cube.vertices.emplace_back(glm::vec3(left, top, far));
			cube.vertices.emplace_back(glm::vec3(left, bottom, far));
			cube.vertices.emplace_back(glm::vec3(right, bottom, far));
			emit_indices(cube.triangles, cube.vertices.size() - 4);

			cube.foreach_vertex([&color](Vertex& v) { v.color = color; }); 

			cube.calculate_vertex_normals();
			
			return cube; 
		}

	}
}
