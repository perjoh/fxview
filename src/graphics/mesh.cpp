#include <GL/glew.h>
#include <vector>
#include "check_opengl_error.hpp"
#include <glm/mat4x4.hpp>
#include "mesh.hpp"
#include "shader.hpp"

namespace graphics {
namespace mesh {

	const char* vertex_shader_src = R"shader(
#version 330
	layout(location=0) in vec3 position; 
	layout(location=1) in vec3 normal;
	layout(location=2) in vec3 color;

	uniform mat4 model_view_projection;
	//uniform mat4 viewProjection;

	out vec3 output_color;

	void main() 
	{
		gl_Position = model_view_projection*vec4(position, 1.0f);

		mat3 tmp = mat3(modelTransform);

		vec3 normalTransformed = tmp*normal;

		vec3 lightpos = vec3(25.0f, 50.0f, 0.0f);
		float shade = dot(normalize(lightpos-position), normalTransformed);

		output_color = min(color+shade, vec3(1.0f, 1.0f, 1.0f));
	} 
	)shader";

	const char* fragment_shader_src = R"shader(
#version 330
	in vec3 input_color;
	out vec4 output_color; 

	void main() {
		output_color=vec4(input_color, 1.0f);
	} 
	)shader";


	class mesh_instance
	{
	public :
		mesh_instance()
		{ } 

		mesh_instance(mesh_instance&& other)
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

		~mesh_instance()
		{
			destroy();
		} 

		void destroy()
		{
			if (element_buffer_object_ != 0) 
			{
				::glDeleteBuffers(1, &element_buffer_object_);
				element_buffer_object_ = 0;
			}

			if (vertex_buffer_object_ != 0) 
			{
				::glDeleteBuffers(1, &vertex_buffer_object_);
				vertex_buffer_object_ = 0;
			}

			if (vertex_array_object_ != 0) 
			{
				::glDeleteVertexArrays(1, &vertex_array_object_);
				vertex_array_object_ = 0;
			} 
		}

		mesh_instance(const mesh_instance&) = delete;
		mesh_instance& operator=(const mesh_instance&) = delete; 


		bool is_valid() const
		{
			return vertex_array_object_ != 0;
		}

		void set_draw_mode(GLenum draw_mode)
		{
			draw_mode_ = draw_mode;
		} 

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
			assert(indices);

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


		void draw()
		{
			::glBindVertexArray(vertex_array_object_); 

			if (0 == element_buffer_object_)
			{
				// Individual triangles
				::glDrawArrays(	draw_mode_, 
								0, 
								render_count_); 
			}
			else
			{
				// Indexed triangles
				::glDrawElements(	GL_TRIANGLES, 
									render_count_, 
									GL_UNSIGNED_INT, 
									0);
			}

			::glBindVertexArray(0);
		}


	private :
		GLuint vertex_buffer_object_{0};
		GLuint vertex_array_object_{0};
		GLuint element_buffer_object_{0};

		GLuint render_count_{0};

		GLenum draw_mode_{GL_TRIANGLES};
	};


	class instance 
	{
	private :
		instance() 
			: shader_{	shader(GL_VERTEX_SHADER, vertex_shader_src), 
						shader(GL_FRAGMENT_SHADER, fragment_shader_src)}
			, mvp_uniform_{shader_.get_uniform<glm::mat4x4>("model_view_projection")}
		{ 
		}

	public :
		static instance& get()
		{
			static instance this_;
			return this_;
		}

		inline unsigned allocate_mesh()
		{ 
			for (unsigned i = 0; i < allocated_meshes_.size(); ++i)
			{
				if (!allocated_meshes_[i].is_valid())
					return i;
			}

			allocated_meshes_.push_back(mesh_instance());
			return allocated_meshes_.size() - 1;
		}

		inline void free_mesh(unsigned i)
		{
			if (i < allocated_meshes_.size())
			{
				if (i == allocated_meshes_.size() - 1)
					allocated_meshes_.pop_back();
				else
					allocated_meshes_[i].destroy();
			}
		}

		mesh_instance& get_mesh(unsigned i)
		{
			return allocated_meshes_[i];
		}

		inline void draw(	unsigned mesh_id, 
							const glm::mat4& transform, 
							const glm::vec3& color)
		{
			shader_.bind();
			mvp_uniform_.set(transform*view_transform_*projection_transform_);

			get_mesh(mesh_id).draw();

			shader_.unbind();
		}

	public :
		std::vector<mesh_instance> allocated_meshes_; 

		shader_program shader_;
		uniform<glm::mat4x4> mvp_uniform_; 

		glm::mat4x4 projection_transform_;
		glm::mat4x4 view_transform_;
	};

	inline instance& instance() { return instance::get(); }







	unsigned alloc_triangles(	const vertex* vertices,
								size_t vertex_count,
								GLenum draw_mode = GL_TRIANGLES)
	{
		unsigned i = instance().allocate_mesh();
		mesh_instance& m = instance().get_mesh(i);

		try
		{
			m.setup_vertex_buffer_object(vertices, vertex_count);
			m.setup_vertex_array_object();
			m.set_draw_mode(GL_TRIANGLE_STRIP);
		}
		catch (const std::exception&)
		{
			instance().free_mesh(i);
			return 0;
		}

		return i; 
	}


	unsigned alloc_triangle_strip(	const vertex* vertices, 
									size_t vertex_count)
	{
		return alloc_triangles(vertices, vertex_count, GL_TRIANGLE_STRIP);
	}


	unsigned alloc_indexed_triangles(	const vertex* vertices, 
										size_t vertex_count, 
										const GLuint* indices, 
										size_t index_count)
	{
		return 0;
	}


	void free(unsigned mesh_id)
	{
		instance().free_mesh(mesh_id);
	} 


	void set_projection_transform(const glm::mat4x4& proj_transform)
	{
		instance().projection_transform_ = proj_transform;
	}


	void set_view_transform(const glm::mat4x4& view_transform)
	{
		instance().view_transform_ = view_transform;
	}


	void draw(	unsigned mesh_id, 
				const glm::mat4& transform, 
				const glm::vec3& color)
	{
		instance().draw(mesh_id, transform, color);
	}

}}
