#include "graphics/mesh.hpp"
#include "graphics/mesh_gen.hpp"
#include "graphics/render.hpp"
#include "graphics/bezier.hpp"
#include "graphics/bezier_render.hpp"
#include "base/task_runner.hpp"
#include "base/frame_time.hpp"
#include "input/event_handler.hpp"
#include <iostream>
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cassert>
#include <vector>
#include <glm/gtx/transform.hpp>

using namespace kvant;

	unsigned mesh_id = graphics::Renderer::invalid_mesh_id;

	void setup_my_render()
	{
		mesh_id = graphics::Renderer::instance().allocate_mesh(graphics::generate_grid(64, 64, 1.0f)); 
	}

	struct Entity
	{ 
		glm::vec3 position;
		float rotation;
		float radius;
	};

	class Entity_container
	{
	public :
		void add();
		void remove();

	public :
		template <typename Fun>
		void for_each(Fun& f)
		{
			for (e : entities_)
			{
				f(e);
			} 
		}

		template <typename Fun>
		void for_each(Fun& f) const
		{
			for (e : entities_)
			{
				f(e);
			} 
		}

	private : 
	};

	class Entity_renderer
	{
	public :
		Entity_renderer()
			: render_mesh_id_(alloc_mesh())
			, shader_(graphics::Renderer::instance().allocate_shader_program("basic"))
		{ 
			auto shader(shader_.lock());
			if (shader)
			{
				model_view_projection_ = shader->get_uniform<glm::mat4>("model_view_projection");
				model_transform_ = shader->get_uniform<glm::mat3>("mode_transform");
			}

			graphics::register_render_callback<Entity_renderer, &Entity_renderer::render>(this);
		}

		void render()
		{ 
			graphics::Shader_scope shader_scope(shader_);
			if (shader_scope)
			{ 
				const glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

				const glm::mat4 view = glm::lookAt(glm::vec3(5, 50, 10),
												   glm::vec3(0, 0, 0),
												   glm::vec3(0, 1, 0));

				const glm::mat4 model(1.0f);

				model_view_projection_.set(model*view*projection);
				model_transform_.set(model);

				graphics::Renderer::instance().render_mesh(render_mesh_id_);
			}
		}

	private :
		unsigned alloc_mesh()
		{
			return graphics::Renderer::instance().allocate_mesh(graphics::generate_smooth_cube(glm::vec3(1.0f)));
		}

	private :
		unsigned render_mesh_id_;
		std::weak_ptr<graphics::Shader_program> shader_;
		graphics::Shader_uniform<glm::mat4> model_view_projection_;
		graphics::Shader_uniform<glm::mat3> model_transform_;
	};

	class Player_entity
	{
	public :
		void set_target(Entity* e)
		{
			entity_ = e;
		}

	private : 
		void update()
		{ 
			// Read controller input.
			// Apply movement.

			if (entity_)
			{

			}
		}

	private :
		Entity* entity_{ nullptr };
	};

	class Camera_entity
	{
	public :
		void set_target(const Entity* e)
		{
			target_ = e;
		}

	private : 
		void update()
		{ 
			if (target_)
			{ 
				// Perform movement based on target.

				//Renderer::instance().set_camera();
			} 
		} 

	private :
		const Entity* target_{ nullptr };
	};

	void my_render()
	{
		/*assert(mesh_id != graphics::Renderer::invalid_mesh_id);

		using namespace graphics;

		//Mesh_renderer& mesh_renderer = Renderer::instance().mesh_renderer();

		const glm::mat4 persp{ glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f) };
		//Renderer::instance().set_projection_transform(persp);

		const glm::mat4 view = glm::lookAt(glm::vec3(5, 50, 10),
										   glm::vec3(0, 0, 0),
										   glm::vec3(0, 1, 0));

		mesh_renderer.set_view_transform(view);

		static float ang = 0.0f;
		auto transform = glm::rotate(ang, glm::vec3(0.0f, 1.0f, 0.0f));
		ang += base::Frame_time::const_instance().delta_time_sec()*glm::pi<double>()*0.5;
		mesh_renderer.render(mesh_id, transform, glm::vec3(1.0f, 0.0f, 0.0f)); */
	}

	class Anim_task
	{
	public:
		Anim_task()
		{
			using namespace kvant::base; 
			Task_runner::instance().add_task(Task_runner::Task_delegate::construct<Anim_task, &Anim_task::Update>(this));
		}

	public :
		void Update()
		{
		}

	private :
	};


int main()
{
	try
	{
		graphics::Renderer::instance().create_windowed(800, 600, "Hello world");

		setup_my_render();

		for (;;)
		{
			graphics::Renderer::instance().begin_render(); 

			my_render(); 

			const bool keep_going = input::Event_handler::process();

			kvant::base::Task_runner::instance().run();

			graphics::Renderer::instance().present();

			kvant::base::Frame_time::instance().next_frame();

			if (!keep_going)
				break;
		}

		graphics::Renderer::instance().destroy();
	}
	catch (const std::exception& e)
	{
		std::cerr << "EXCEPTION CAUGHT: " << e.what() << std::endl;
	}

	return 0;
}

