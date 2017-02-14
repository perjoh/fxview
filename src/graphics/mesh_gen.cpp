#include "mesh_gen.hpp"
#include <cassert>

namespace graphics
{
	namespace mesh
	{ 
		static const unsigned NUM_VERTICES_CUBE = 36;

		triangle_array triangulate_shape(	unsigned num_segments, 
											unsigned segment_size)
		{
			assert(num_segments > 1);
			assert(segment_size > 1);

			triangle_array triangles;
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
		point_array generate_cube(const glm::vec3& size)
		{
			point_array cube;
			cube.reserve(NUM_VERTICES_CUBE);

			const float s = 0.5f;

			// Near
			cube.emplace_back(point(-s, s, s)*size);
			cube.emplace_back(point(s, s, s)*size);
			cube.emplace_back(point(s, -s, s)*size);
			cube.emplace_back(point(s, -s, s)*size);
			cube.emplace_back(point(-s, -s, s)*size);
			cube.emplace_back(point(-s, s, s)*size);

			// Left
			cube.emplace_back(point(-s, s, -s)*size);
			cube.emplace_back(point(-s, s, s)*size);
			cube.emplace_back(point(-s, -s, s)*size);
			cube.emplace_back(point(-s, -s, s)*size);
			cube.emplace_back( point(-s, -s, -s)*size);
			cube.emplace_back( point(-s, s, -s)*size);

			// Far
			cube.emplace_back(point(s, s, -s)*size);
			cube.emplace_back(point(-s, s, -s)*size);
			cube.emplace_back(point(-s, -s, -s)*size);
			cube.emplace_back(point(-s, -s, -s)*size);
			cube.emplace_back(point(s, -s, -s)*size);
			cube.emplace_back(point(s, s, -s)*size);

			// Right
			cube.emplace_back(point(s, s, s)*size);
			cube.emplace_back(point(s, s, -s)*size);
			cube.emplace_back(point(s, -s, -s)*size);
			cube.emplace_back(point(s, -s, -s)*size);
			cube.emplace_back(point(s, -s, s)*size);
			cube.emplace_back(point(s, s, s)*size);

			// Top
			cube.emplace_back(point(-s, s, -s)*size);
			cube.emplace_back(point(s, s, -s)*size);
			cube.emplace_back(point(s, s, s)*size);
			cube.emplace_back(point(s, s, s)*size);
			cube.emplace_back(point(-s, s, s)*size);
			cube.emplace_back(point(-s, s, -s)*size);

			// Bottom
			cube.emplace_back(point(-s, -s, s)*size);
			cube.emplace_back(point(s, -s, s)*size);
			cube.emplace_back(point(s, -s, -s)*size);
			cube.emplace_back(point(s, -s, -s)*size);
			cube.emplace_back(point(-s, -s, -s)*size);
			cube.emplace_back(point(-s, -s, s)*size);
			
			return cube; 
		}

		//
		unsigned allocate_cube(	const glm::vec3& size, 
								const glm::vec3& color)
		{
			auto points = generate_cube(size);

			const float s = 0.5f;
			std::vector<graphics::mesh::vertex> vertices(points.size());

			for (unsigned i = 0; i < points.size(); ++i)
			{
				vertices[i].position = points[i]; 
				vertices[i].color = color;
			} 

			calc_vertex_normals(&vertices[0], vertices.size());

			return alloc_triangles(&vertices[0], vertices.size());
		}
	}
}
