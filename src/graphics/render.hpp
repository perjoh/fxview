#pragma once
#include "mesh.hpp"
#include "shader.hpp"
#include <glm/mat4x4.hpp>
#include <GL/glew.h>

struct SDL_Window;
typedef void* SDL_GLContext;

namespace graphics {
namespace render { 

	class Internal_mesh
	{
	public :
		Internal_mesh() = default; 
		Internal_mesh(Internal_mesh&& other); 
		~Internal_mesh();

		Internal_mesh(const Internal_mesh&) = delete;
		Internal_mesh& operator=(const Internal_mesh&) = delete; 

	public :
		void destroy(); 

		bool is_valid() const;

		void set_draw_mode(GLenum draw_mode); 

		typedef graphics::mesh::Vertex Vertex;
		void setup_vertex_buffer_object(const Vertex* vertices, size_t vertex_count); 
		void setup_element_buffer_object(const GLuint* indices, size_t index_count); 
		void setup_vertex_array_object(); 

		void render();

	private :
		GLuint vertex_buffer_object_{0};
		GLuint vertex_array_object_{0};
		GLuint element_buffer_object_{0};

		GLuint render_count_{0};

		GLenum draw_mode_{GL_TRIANGLES};
	};

	class Mesh_renderer
	{
	public :
		Mesh_renderer();

	public :
		static const unsigned invalid_mesh_id{ ~0ul };

		typedef unsigned Mesh_id;
		Mesh_id allocate_mesh(const graphics::mesh::Triangle_mesh<>&);
		void deallocate_mesh(Mesh_id mesh_id);

		void set_projection_transform(const glm::mat4&);
		void set_view_transform(const glm::mat4&);

		void render(Mesh_id mesh_id, const glm::mat4& transform, const glm::vec3& color);

	private : 
		std::vector<Internal_mesh> allocated_meshes_; 
		unsigned num_free_{ 0 };

		Shader_program shader_;
		Uniform<glm::mat4> mvp_transform_; 
		Uniform<glm::mat3> model_transform_; 
		//Uniform<glm::vec3> color_;

		glm::mat4 projection_transform_;
		glm::mat4 view_transform_; 
	};

	class Renderer
	{
	public :
		static Renderer& instance();

		void create(unsigned window_width, 
					unsigned window_height, 
					const char* window_title); 

		void destroy();

	public :
		void begin_render();
		void end_render();

		inline Mesh_renderer& mesh_renderer()
		{
			return mesh_renderer_; 
		}

	private : 
		Renderer() = default;

		SDL_Window* window_{ nullptr };
		SDL_GLContext context_{ nullptr }; 

	private :
		Mesh_renderer mesh_renderer_;
	};



		/*using graphics::mesh::Vertex;

		static_assert(sizeof(glm::vec3) == sizeof(float)*3, "glm::vec3 should be 12 bytes");
		static_assert(sizeof(Vertex) == sizeof(glm::vec3)*3, "vertex structure should be packed");
		static_assert(sizeof(Vertex) == sizeof(glm::vec3)*3, "stride is not correct");*/




		/*unsigned alloc_triangle_strip(	const Vertex* vertices, 
										size_t vertex_count);

		unsigned alloc_indexed_triangles(	const Vertex* vertices, 
											size_t vertex_count, 
											const GLuint* indices, 
											size_t index_count);

		unsigned alloc_triangles(	const Vertex* vertices, 
									size_t vertex_count,
									GLenum draw_mode = GL_TRIANGLES);*/

		//unsigned allocate_mesh(const graphics::mesh::Triangle_mesh<Vertex>&);
		//void deallocate_mesh(unsigned);

		//void set_projection_transform(const glm::mat4&);
		//void set_view_transform(const glm::mat4&);

		//void draw(	unsigned mesh_id, const glm::mat4& transform, const glm::vec3& color);

}}
