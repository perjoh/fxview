#include <GL/glew.h>
#include <vector>
#include "check_opengl_error.hpp"
#include <glm/mat4x4.hpp>
#include "mesh.hpp"

namespace graphics {
namespace mesh {

	const char* vertex_shader_src = R"shader(
#version 330
layout(location=0) in vec3 position; 
layout(location=1) in vec3 normal;
layout(location=2) in vec3 color;

uniform mat4 modelTransform;
uniform mat4 viewProjection;

out vec3 thecolor;

void main() 
{
	gl_Position = viewProjection*modelTransform*vec4(position, 1.0f);

	mat3 tmp = mat3(modelTransform);

	vec3 normalTransformed = tmp*normal;

	vec3 lightpos = vec3(25.0f, 50.0f, 0.0f);
	float shade = dot(normalize(lightpos-position), normalTransformed);

	thecolor = min(color+shade, vec3(1.0f, 1.0f, 1.0f));
} 
	)shader";

	const char* fragment_shader_src = R"shader(
#version 330
out vec4 outputColor;

in vec3 thecolor;

void main() {
	outputColor=vec4(thecolor, 1.0f);
} 
	)shader";



	class internal_mesh
	{
	public :
		internal_mesh()
		{ } 

		internal_mesh(internal_mesh&& other)
			: vertex_buffer_object_{other.vertex_buffer_object_}
			, vertex_array_object_{other.vertex_array_object_}
			, element_buffer_object_{other.element_buffer_object_}
			, render_count_{other.render_count_}
		{
			other.vertex_buffer_object_ = 0;
			other.vertex_array_object_ = 0;
			other.element_buffer_object_ = 0; 
			other.render_count_ = 0;
		} 

		~internal_mesh()
		{
			if (element_buffer_object_ != 0) 
			{
				::glDeleteBuffers(1, &element_buffer_object_);
			}

			if (vertex_buffer_object_ != 0) 
			{
				::glDeleteBuffers(1, &vertex_buffer_object_);
			}

			if (vertex_array_object_ != 0) 
			{
				::glDeleteVertexArrays(1, &vertex_array_object_);
			} 
		} 

		// prohibit copying
		internal_mesh(const internal_mesh&) = delete;
		internal_mesh& operator=(const internal_mesh&) = delete; 

		void setup_vertex_buffer_object(const vertex* vertices, size_t vertex_count) 
		{
			::glGenBuffers(1, &vertex_buffer_object_); 

			::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_); 

			const GLsizeiptr dataSize = sizeof(vertex)*vertex_count;

			::glBufferData(	GL_ARRAY_BUFFER, 
							dataSize, 
							&vertices[0], 
							GL_STATIC_DRAW );

			::glBindBuffer(GL_ARRAY_BUFFER, 0); 

			check_opengl_error();

			render_count_ = vertex_count;
		}

		void setup_element_buffer_object(const GLuint* indices, size_t index_count) 
		{
			if (indices) 
			{ 
				::glGenBuffers(1, &element_buffer_object_); 
				::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);

				const size_t index_buffer_size = sizeof(GLuint)*index_count;
				::glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
								index_buffer_size, 
								&indices[0], 
								GL_STATIC_DRAW ); 

				::glBindBuffer(	GL_ELEMENT_ARRAY_BUFFER, 
								0); 

				render_count_ = index_count; // ???

				check_opengl_error();
			} 
		}

		void setup_vertex_array_object() 
		{ 
			assert(vertex_array_object_ == 0);
			assert(vertex_buffer_object_ != 0);

			::glGenVertexArrays(1, &vertex_array_object_);
			::glBindVertexArray(vertex_array_object_);

			// Vertex Buffer Object
			::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_); 

			// Element Buffer Object
			if (element_buffer_object_ != 0)
			{
				::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
			}

			for (unsigned index = 0, offset = 0; index < 3; ++index)
			{ 
				::glEnableVertexAttribArray(index); 

				const GLboolean normalize = GL_FALSE; 
				::glVertexAttribPointer(index, 
										3, 
										GL_FLOAT, 
										normalize, 
										vertex::stride, 
										reinterpret_cast<const GLvoid*>(offset)); 

				offset += sizeof(glm::vec3);
			}

			::glBindVertexArray(0); 

			check_opengl_error(); 
		} 

	private :
		GLuint vertex_buffer_object_{0};
		GLuint vertex_array_object_{0};
		GLuint element_buffer_object_{0};

		GLuint render_count_{0};
	};

	unsigned alloc_triangle_strip(	const vertex* vertices, 
									size_t vertex_count)
	{
		return 0;
	}


	unsigned alloc_indexed_triangles(	const vertex* vertices, 
										size_t vertex_count, 
										const GLuint* indices, 
										size_t index_count)
	{
		return 0;
	}


	unsigned alloc_triangles(	const vertex* vertices, 
								size_t vertex_count)
	{
		return 0;
	}


	void free(unsigned mesh_id)
	{
	}
	

	static std::vector<internal_mesh> g_allocated_meshes;


	void draw_mesh(unsigned mesh_id, const glm::mat4x4& transform, const glm::vec3& color)
	{
	}

}}
