#pragma
#include "mesh.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace graphics
{
	namespace mesh
	{
		//
		inline glm::vec3 calc_normal(	const glm::vec3& v0, 
										const glm::vec3& v1, 
										const glm::vec3& v2) 
		{ 
			return glm::normalize(glm::cross(v2-v0, v1-v0));
		}

		struct triangle 
		{
			triangle() = default;

			triangle(	unsigned v0_, 
						unsigned v1_, 
						unsigned v2_)
				: v0(v0_)
				, v1(v1_)
				, v2(v2_)
			{ }

			unsigned v0{ ~0u };
			unsigned v1{ ~0u };
			unsigned v2{ ~0u };
		};

		typedef std::vector<triangle> triangle_array;

		// Calculate vertex normals "in place".
		template <typename vertex_type>
		void calc_vertex_normals(	vertex_type* vertices, 
									unsigned num_vertices, 
									const triangle_array& triangles)
		{ 
			std::vector<unsigned> denom(num_vertices, 0);
			for (const triangle& t : triangles)
			{ 
				auto p0{ vertices[t.v0].position };
				auto p1{ vertices[t.v1].position };
				auto p2{ vertices[t.v2].position };

				auto normal = calc_normal(p0, p1, p2);

				vertices[t.v0].normal += normal;
				vertices[t.v1].normal += normal;
				vertices[t.v2].normal += normal;

				denom[t.v0] += 1;
				denom[t.v1] += 1;
				denom[t.v2] += 1;
			}

			for (unsigned i = 0; i < denom.size(); ++i)
			{
				vertices[i].normal /= static_cast<float>(denom[i]); 
				glm::normalize(vertices[i].normal);
			}
		}

		//
		template <typename vertex_type>
		void calc_vertex_normals(	vertex_type* vertices, 
									unsigned num_vertices)
		{ 
			assert(!(num_vertices < 3));
			for (unsigned i = 0; i < num_vertices; i += 3)
			{ 
				auto p0{ vertices[i + 0].position };
				auto p1{ vertices[i + 1].position };
				auto p2{ vertices[i + 2].position };
				auto normal = calc_normal(p0, p1, p2);

				vertices[i + 0].normal = normal;
				vertices[i + 1].normal = normal;
				vertices[i + 2].normal = normal;
			}
		}

		//
		template <typename vertex_type>
		void set_color(	vertex_type* vertices, 
						unsigned num_vertices, 
						const glm::vec3& color)
		{
			for (unsigned i = 0; i < num_vertices; ++i)
			{
				vertices[i].color = color;
			} 
		}

		typedef glm::vec3 point;
		typedef std::vector<point> point_array; 

		//
		template <typename iter_type>
		inline void transform_points_inplace(	iter_type begin, 
												iter_type end,
												const glm::mat4& m)
		{ 
			for (iter_type point_it = begin; point_it != end; ++point_it)
			{ 
				*point_it = (m*glm::vec4(	point_it->x, 
											point_it->y, 
											point_it->z, 
											1.0f));
			}
		}

		//
		inline point_array transform_points(const point_array& points, 
											const glm::mat4& m)
		{ 
			point_array tmp(points.size());
			transform_points_inplace(tmp.begin(), tmp.end(), m); 
			return tmp;
		}

		namespace shaper
		{
			// Esoteric stuff related to generating a mesh based on bezier curves.

			//
			template <typename radius_curve_type>
			point_array generate_segment(const radius_curve_type& radius,
										 unsigned num_tiles)
			{
				point_array points;
				points.reserve(num_tiles);

				float t = 0.0f;
				const float time_step = 1.0f / static_cast<float>(num_tiles);
				for (unsigned tile = 0; tile < num_tiles; ++tile)
				{
					point p = radius.sample(t);
					points.push_back(p);
					t += time_step;
				}
				return points;
			}

			//
			template <	typename radius_curve_type,
				typename path_curve_type,
				typename scale_curve_type>
				point_array generate_shape(const radius_curve_type& radius_curve,
										   const path_curve_type& path_curve,
										   const scale_curve_type& scale_curve,
										   unsigned num_segments,
										   unsigned segment_size)
			{
				const point_array segment(generate_segment(radius_curve, segment_size));

				point_array shape(num_segments*segment_size);
				auto shape_it = shape.begin();

				// Sample along path curve.
				float t = 0.0f;
				const float delta_t = 1.0f / static_cast<float>(num_segments - 1);
				for (unsigned i = 0;
					 i < num_segments;
					 ++i)
				{
					auto scaling = scale_curve.sample(t);
					auto sampled_path = path_curve.sample(scaling.y);
					//glm::mat4 transform{ glm::scale(glm::mat4(1.0f), glm::vec3(scale.x)) };
					//glm::mat4 transform = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale.x)), sampled_path);
					//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(scaling.x));
					glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.0f), sampled_path), glm::vec3(scaling.x));

					std::copy(segment.cbegin(),
							  segment.cend(),
							  shape_it);

					transform_points_inplace(shape_it,
											 shape_it + segment_size,
											 transform);

					shape_it += segment_size;
					t += delta_t;
				}

				return shape;
			}

			//
			inline triangle_array triangulate_shape(unsigned num_segments, 
													unsigned segment_size)
			{
				assert(num_segments > 1);
				assert(segment_size > 1);

				triangle_array triangles;
				triangles.reserve((num_segments - 1)*segment_size * 2);
				for (unsigned segment = 0;
					 segment < (num_segments - 1);
					 ++segment)
				{
					for (unsigned i = 0;
						 i < segment_size;
						 ++i)
					{
						const unsigned offset = segment_size*segment;
						auto wrap = [segment_size](unsigned i) { return i%segment_size; };

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

		}
 
		//
		point_array generate_cube(const glm::vec3& size);

		//
		unsigned allocate_cube(const glm::vec3& size, const glm::vec3& color);

	}
}
