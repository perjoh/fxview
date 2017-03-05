#pragma
#include "mesh.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace graphics {
namespace mesh {

		typedef glm::vec3 Point;
		typedef std::vector<Point> Point_array; 

		//
		template <typename Iterator>
		inline void TransformPointsInPlace(	Iterator begin, 
											Iterator end,
											const glm::mat4& m)
		{ 
			for (Iterator point_it = begin; point_it != end; ++point_it)
			{ 
				*point_it = (m*glm::vec4(	point_it->x, 
											point_it->y, 
											point_it->z, 
											1.0f));
			}
		}

		//
		inline Point_array TransformPoints(const Point_array& points, 
											const glm::mat4& m)
		{ 
			Point_array tmp(points.size());
			TransformPointsInPlace(tmp.begin(), tmp.end(), m); 
			return tmp;
		}

		namespace shaper
		{
			// Esoteric stuff related to generating a mesh based on bezier curves.

			//
			template <typename radius_curve_type>
			Point_array generate_segment(const radius_curve_type& radius,
										 unsigned num_tiles)
			{
				Point_array points;
				points.reserve(num_tiles);

				float t = 0.0f;
				const float time_step = 1.0f / static_cast<float>(num_tiles);
				for (unsigned tile = 0; tile < num_tiles; ++tile)
				{
					Point p = radius.sample(t);
					points.push_back(p);
					t += time_step;
				}
				return points;
			}

			//
			template <	typename radius_curve_type,
				typename path_curve_type,
				typename scale_curve_type>
				Point_array generate_shape(const radius_curve_type& radius_curve,
										   const path_curve_type& path_curve,
										   const scale_curve_type& scale_curve,
										   unsigned num_segments,
										   unsigned segment_size)
			{
				const Point_array segment(generate_segment(radius_curve, segment_size));

				Point_array shape(num_segments*segment_size);
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

					TransformPointsInPlace(shape_it,
											 shape_it + segment_size,
											 transform);

					shape_it += segment_size;
					t += delta_t;
				}

				return shape;
			}

			//
			inline Triangle_array triangulate_shape(unsigned num_segments, 
													unsigned segment_size)
			{
				assert(num_segments > 1);
				assert(segment_size > 1);

				Triangle_array triangles;
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
		//Point_array GenerateCube(const glm::vec3& size);

		//
		Triangle_mesh<> generate_cube(const glm::vec3& size, const glm::vec3& color);

	}
}
