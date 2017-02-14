#include "mesh.hpp"
#include "check_opengl_error.hpp"
#include "shader.hpp"
#include <vector>

namespace graphics {
namespace mesh {

	const char* vertex_shader_src = R"shader(
#version 330
	layout(location=0) in vec3 position; 
	layout(location=1) in vec3 normal; 
	layout(location=2) in vec3 color; 
	uniform mat4 model_view_projection;
	uniform vec3 in_color;
	out vec3 the_color;

	void main() 
	{
		gl_Position = model_view_projection*vec4(position, 1.0f);
		the_color = in_color*color;
	} 
	)shader";

	const char* fragment_shader_src = R"shader(
#version 330
	in vec3 the_color;
	out vec4 output_color; 

	void main() {
		output_color=vec4(the_color, 1.0f);
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

			check_opengl_error(); 

			// Vertex Buffer Object
			::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_); 

			check_opengl_error(); 

			// Element Buffer Object
			if (element_buffer_object_ != 0)
			{
				::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
			}

			const unsigned NUM_ARRAYS = 3;
			for (unsigned index = 0, offset = 0; index < NUM_ARRAYS; ++index)
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

				check_opengl_error(); 
			}

			::glBindVertexArray(0); 
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

			check_opengl_error();

			::glBindVertexArray(0);
		}


	private :
		GLuint vertex_buffer_object_{0};
		GLuint vertex_array_object_{0};
		GLuint element_buffer_object_{0};

		GLuint render_count_{0};

		GLenum draw_mode_{GL_TRIANGLES};
	};


	class mesh_module 
	{
	private :
		mesh_module() 
			: shader_{	shader(GL_VERTEX_SHADER, vertex_shader_src), 
						shader(GL_FRAGMENT_SHADER, fragment_shader_src)}
			, mvp_uniform_{shader_.get_uniform<glm::mat4x4>("model_view_projection")}
			, color_{shader_.get_uniform<glm::vec3>("in_color")}
		{ 
		}

	public :
		static mesh_module& get_instance()
		{
			static mesh_module this_;
			return this_;
		}

		inline unsigned allocate_mesh()
		{ 
			for (unsigned i = 0; i < allocated_meshes_.size(); ++i)
			{
				if (!allocated_meshes_[i].is_valid())
					return i;
			}

			allocated_meshes_.emplace_back();
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
			assert(i < allocated_meshes_.size());
			return allocated_meshes_[i];
		}

		inline void draw(	unsigned mesh_id, 
							const glm::mat4& transform, 
							const glm::vec3& color)
		{
			shader_.bind();
			mvp_uniform_.set(projection_transform_*view_transform_*transform);
			color_.set(glm::vec3(1.0f, 0.0f, 0.0f));

			get_mesh(mesh_id).draw();

			shader_.unbind();
		}

	public :
		std::vector<mesh_instance> allocated_meshes_; 

		shader_program shader_;
		uniform<glm::mat4x4> mvp_uniform_; 
		uniform<glm::vec3> color_;

		glm::mat4x4 projection_transform_;
		glm::mat4x4 view_transform_;
	};

	inline mesh_module& module_instance() { return mesh_module::get_instance(); }


 unsigned alloc_triangles(	const vertex* vertices,
							size_t vertex_count,
							GLenum draw_mode)
	{
		unsigned i = module_instance().allocate_mesh();
		mesh_instance& m = module_instance().get_mesh(i);

		m.setup_vertex_buffer_object(vertices, vertex_count);
		m.setup_vertex_array_object();
		m.set_draw_mode(draw_mode);

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
		unsigned i = module_instance().allocate_mesh();
		mesh_instance& m = module_instance().get_mesh(i);

		m.setup_vertex_buffer_object(vertices, vertex_count);
		m.setup_element_buffer_object(indices, index_count);
		m.setup_vertex_array_object();
		//m.set_draw_mode(draw_mode);

		return i; 
	}


	void free(unsigned mesh_id)
	{
		module_instance().free_mesh(mesh_id);
	} 


	void set_projection_transform(const glm::mat4x4& proj_transform)
	{
		module_instance().projection_transform_ = proj_transform;
	}


	void set_view_transform(const glm::mat4x4& view_transform)
	{
		module_instance().view_transform_ = view_transform;
	}


	void draw(	unsigned mesh_id, 
				const glm::mat4& transform, 
				const glm::vec3& color)
	{
		module_instance().draw(mesh_id, transform, color);
		check_opengl_error();
	}

}}
