#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace graphics {
namespace mesh {

		//
		inline glm::vec3 calculate_normal(const glm::vec3& v0,
										  const glm::vec3& v1,
										  const glm::vec3& v2)
		{
			return glm::normalize(glm::cross(v2 - v0, v1 - v0));
		}

		struct Vertex {

			Vertex() = default;
			
			Vertex(const glm::vec3& pos)
				: position(pos)
			{ }

			Vertex(	const glm::vec3& pos, 
					const glm::vec3& norm, 
					const glm::vec3& col)
				: position(pos)
				, normal(norm)
				, color(col)
			{ }

			glm::vec3 position;
			glm::vec3 normal;
			glm::vec3 color;
		};

		struct Triangle {
			Triangle() = default;

			Triangle(unsigned v0_,
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

		typedef std::vector<Triangle> Triangle_array;

		//
		template <typename Vertex = graphics::mesh::Vertex>
		struct Triangle_mesh {

			Triangle_mesh() = default;

			Triangle_mesh(Triangle_mesh&& other)
				: vertices(std::move(other.vertices))
				, triangles(std::move(other.triangles))
			{ }

			std::vector<Vertex> vertices;
			Triangle_array triangles;

			template <typename Fun>
			void for_each_vertex(Fun& f)
			{
				for (auto& vertex : vertices)
				{
					f(vertex);
				} 
			}

			//
			void transform(const glm::mat4& m)
			{
				for_each_vertex([&m](Vertex& v) { v.position = m*v.position; });
			}

			//
			void calculate_vertex_normals()
			{
				std::vector<unsigned> denom(vertices.size(), 0);

				for (const Triangle& t : triangles)
				{
					auto p0{ vertices[t.v0].position };
					auto p1{ vertices[t.v1].position };
					auto p2{ vertices[t.v2].position };

					auto normal = calculate_normal(p0, p1, p2);

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

			void set_color(const glm::vec3& color)
			{
				for_each_vertex([&color](Vertex& v) { v.color = color; });
			}

		};

} }
