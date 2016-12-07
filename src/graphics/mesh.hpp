#pragma once

namespace graphics {
namespace mesh { 

		struct vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec3 color;
			static const GLuint stride = sizeof(position) + sizeof(normal) + sizeof(color);
		};

		static_assert(sizeof(glm::vec3) == sizeof(float)*3, "glm::vec3 should be 12 bytes");
		static_assert(sizeof(vertex) == sizeof(glm::vec3)*3, "vertex structure should be packed");


		unsigned alloc_triangle_strip(	const vertex* vertices, 
										size_t vertex_count);

		unsigned alloc_indexed_triangles(	const vertex* vertices, 
											size_t vertex_count, 
											const GLuint* indices, 
											size_t index_count);

		unsigned alloc_triangles(	const vertex* vertices, 
									size_t vertex_count);

		void free(unsigned);

		void set_projection_transform(const glm::mat4x4&);
		void set_view_transform(const glm::mat4x4&);

		void draw(	unsigned mesh_id, 
					const glm::mat4x4& transform, 
					const glm::vec3& color);

}}
