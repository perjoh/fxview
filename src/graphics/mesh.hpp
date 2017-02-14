#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>

namespace graphics {
namespace mesh { 

		struct vertex
		{
			vertex() = default;

			vertex(	const glm::vec3& pos, 
					const glm::vec3& norm, 
					const glm::vec3& col)
				: position(pos)
				, normal(norm)
				, color(col)
			{ }

			glm::vec3 position;
			glm::vec3 normal;
			glm::vec3 color;
			static const GLuint stride = sizeof(vertex::position) + sizeof(vertex::normal) + sizeof(vertex::color);
		};

		static_assert(sizeof(glm::vec3) == sizeof(float)*3, "glm::vec3 should be 12 bytes");
		static_assert(sizeof(vertex) == sizeof(glm::vec3)*3, "vertex structure should be packed");
		static_assert(vertex::stride == sizeof(glm::vec3)*3, "stride is not correct");


		const unsigned invalid_mesh_id = ~0;


		unsigned alloc_triangle_strip(	const vertex* vertices, 
										size_t vertex_count);

		unsigned alloc_indexed_triangles(	const vertex* vertices, 
											size_t vertex_count, 
											const GLuint* indices, 
											size_t index_count);

		unsigned alloc_triangles(	const vertex* vertices, 
									size_t vertex_count,
									GLenum draw_mode = GL_TRIANGLES);

		void free(unsigned);

		void set_projection_transform(const glm::mat4x4&);
		void set_view_transform(const glm::mat4x4&);

		void draw(	unsigned mesh_id, 
					const glm::mat4x4& transform, 
					const glm::vec3& color);

}}
