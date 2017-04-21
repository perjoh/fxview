#include "render.hpp"
#include "check_opengl_error.hpp"
#include <vector>
#include <SDL.h>

namespace graphics {
namespace render {

	Renderer& Renderer::instance()
	{
		static Renderer instance;
		return instance;
	}

	void Renderer::create(	unsigned window_width, 
							unsigned window_height, 
							const char* window_title) 
	{ 
		assert(window_ == nullptr);

		window_ = ::SDL_CreateWindow(	window_title,
										SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										window_width, window_height,
										SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

		if (nullptr == window_) 
		{
			throw std::runtime_error("Failed to create window.");
		}

		context_ = ::SDL_GL_CreateContext(window_);
		if (nullptr == context_) 
		{
			throw std::runtime_error("Failed to create OpenGL context.");
		}

		GLenum error = ::glewInit();
		if (error != GLEW_OK)
		{
			throw std::runtime_error("glewInit failed.");
		} 

		::glEnable(GL_DEPTH_TEST);
		//::glEnable(GL_CULL_FACE);
		//::glCullFace(GL_FRONT);
		//::glCullFace(GL_BACK);
	}


	void Renderer::destroy() 
	{ 
		if (context_)
		{
			::SDL_GL_DeleteContext(context_);
		}

		if (window_) 
		{
			::SDL_DestroyWindow(window_);
		}
	}


	void clear_buffers()
	{
        glClearColor(0.0, 1.0, 0.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}


	void Renderer::begin_render()
	{
		clear_buffers();
	}


	void Renderer::end_render()
	{
		::SDL_GL_SwapWindow(window_);
	}


	const char* vertex_shader_src = R"shader(
#version 330
	layout(location=0) in vec3 position; 
	layout(location=1) in vec3 normal; 
	layout(location=2) in vec3 color; 
	uniform mat4 model_view_projection;
	uniform mat3 model_transform;
	//uniform vec3 in_color;
	out vec3 the_color;

	void main() 
	{
		gl_Position = model_view_projection*vec4(position, 1.0f);
		vec3 lightpos = vec3(25.0f, 50.0f, 0.0f);
		vec3 normal_trans = model_transform*normal;
		float shade = dot(normalize(lightpos - position), normal_trans);
		the_color = min(color + shade, vec3(1.0f, 1.0f, 1.0f));
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


	Internal_mesh::Internal_mesh(Internal_mesh&& other)
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

	Internal_mesh::~Internal_mesh()
	{
		destroy();
	} 

	void Internal_mesh::destroy()
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

	bool Internal_mesh::is_valid() const
	{
		return vertex_array_object_ != 0;
	}

	void Internal_mesh::set_draw_mode(GLenum draw_mode)
	{
		draw_mode_ = draw_mode;
	} 

	void Internal_mesh::setup_vertex_buffer_object(const Vertex* vertices, size_t vertex_count) 
	{
		::glGenBuffers(1, &vertex_buffer_object_); 

		::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_); 

		const GLsizeiptr dataSize = sizeof(Vertex)*vertex_count;

		::glBufferData(	GL_ARRAY_BUFFER, 
						dataSize, 
						&vertices[0], 
						GL_STATIC_DRAW );

		::glBindBuffer(GL_ARRAY_BUFFER, 0); 

		check_opengl_error();

		render_count_ = vertex_count;
	}


	void Internal_mesh::setup_element_buffer_object(const GLuint* indices, size_t index_count) 
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

	void Internal_mesh::setup_vertex_array_object() 
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
		for (	unsigned index = 0, offset = 0; 
				index < NUM_ARRAYS; 
				++index)
		{ 
			::glEnableVertexAttribArray(index); 

			const GLboolean normalize = GL_FALSE; 
			::glVertexAttribPointer(index, 
									3, 
									GL_FLOAT, 
									normalize, 
									sizeof(graphics::mesh::Vertex), 
									reinterpret_cast<const GLvoid*>(offset)); 

			offset += sizeof(glm::vec3);

			check_opengl_error(); 
		}

		::glBindVertexArray(0); 
	} 


	void Internal_mesh::render()
	{
		::glBindVertexArray(vertex_array_object_); 

		if (0 == element_buffer_object_)
		{
			// Individual triangles
			::glDrawArrays(draw_mode_, 0, render_count_); 
		}
		else
		{
			// Indexed triangles
			::glDrawElements(GL_TRIANGLES, render_count_, GL_UNSIGNED_INT, 0);
		}

		check_opengl_error();

		::glBindVertexArray(0);
	} 


	Mesh_renderer::Mesh_renderer() 
		: shader_{	Shader(GL_VERTEX_SHADER, vertex_shader_src), 
					Shader(GL_FRAGMENT_SHADER, fragment_shader_src)}
		, mvp_transform_{shader_.get_uniform<glm::mat4>("model_view_projection")}
		, model_transform_{shader_.get_uniform<glm::mat3>("model_transform")}
		//, color_{shader_.get_uniform<glm::vec3>("in_color")}
	{ 
	}

	unsigned Mesh_renderer::allocate_mesh(const graphics::mesh::Triangle_mesh<>& tri_mesh)
	{ 
		unsigned mesh_id = invalid_mesh_id;

		if (num_free_ > 0)
		{
			for (unsigned i = 0; i < allocated_meshes_.size(); ++i)
			{
				if (!allocated_meshes_[i].is_valid())
				{
					--num_free_;
					mesh_id = i;
					break;
				}
			}
		} 
		else if (invalid_mesh_id == mesh_id)
		{ 
			allocated_meshes_.emplace_back();
			mesh_id = allocated_meshes_.size() - 1;
		}

		Internal_mesh& m = allocated_meshes_[mesh_id];

		m.setup_vertex_buffer_object(&tri_mesh.vertices[0], tri_mesh.vertices.size());

		if (!tri_mesh.triangles.empty()) 
		{
			m.setup_element_buffer_object(&tri_mesh.triangles[0].v0, tri_mesh.triangles.size()*3);
		}

		m.setup_vertex_array_object();

		return mesh_id;
	}

	void Mesh_renderer::deallocate_mesh(Mesh_id i)
	{
		allocated_meshes_[i].destroy();
		++num_free_;
	}

	void Mesh_renderer::set_projection_transform(const glm::mat4& proj_transform)
	{
		projection_transform_ = proj_transform;
	}


	void Mesh_renderer::set_view_transform(const glm::mat4& view_transform)
	{
		view_transform_ = view_transform;
	}

	void Mesh_renderer::render(Mesh_id mesh_id, const glm::mat4& transform, const glm::vec3& color)
	{
		shader_.bind();
		mvp_transform_.set(projection_transform_*view_transform_*transform);
		model_transform_.set(glm::mat3(transform));
		//color_.set(glm::vec3(1.0f, 0.0f, 0.0f));

		allocated_meshes_[mesh_id].render();

		shader_.unbind();
	}


	/*unsigned alloc_triangles(const Vertex* vertices, size_t vertex_count, GLenum draw_mode)
	{
		unsigned i = module_instance().allocate_mesh();
		Mesh_instance& m = module_instance().get_mesh(i);

		m.setup_vertex_buffer_object(vertices, vertex_count);
		m.setup_vertex_array_object();
		m.set_draw_mode(draw_mode);

		return i; 
	}*/


	/*unsigned alloc_triangle_strip(	const Vertex* vertices, 
									size_t vertex_count)
	{
		return alloc_triangles(vertices, vertex_count, GL_TRIANGLE_STRIP);
	}*/


	/*unsigned alloc_indexed_triangles(	const Vertex* vertices, 
										size_t vertex_count, 
										const GLuint* indices, 
										size_t index_count)
	{
		unsigned i = module_instance().allocate_mesh();
		Mesh_instance& m = module_instance().get_mesh(i);

		m.setup_vertex_buffer_object(vertices, vertex_count);
		m.setup_element_buffer_object(indices, index_count);
		m.setup_vertex_array_object();
		//m.set_draw_mode(draw_mode);

		return i; 
	}*/


	/*void deallocate_mesh(unsigned mesh_id)
	{
		module_instance().deallocate_mesh(mesh_id);
	} */



	/*void draw(	unsigned mesh_id, 
				const glm::mat4& transform, 
				const glm::vec3& color)
	{
		module_instance().draw(mesh_id, transform, color);
		check_opengl_error();
	}*/

}}
