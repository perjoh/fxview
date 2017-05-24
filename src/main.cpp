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

	struct Entity
	{ 
		glm::vec3 position{0.0f};
		float rotation{ 0.0f };
		float radius{ 1.0f };
	};

	class Entity_container
	{
	public :
		void add(const Entity& e)
		{
			entities_.push_back(e); 
		}

	public :
		template <typename Fun>
		void for_each(Fun&& f)
		{
			std::for_each(begin(entities_), end(entities_), f);
		}

		template <typename Fun>
		void for_each(Fun&& f) const
		{
			using namespace std;
			std::for_each(cbegin(entities_), cend(entities_), f);
		}

	private : 
		std::vector<Entity> entities_;
	};

	class Entity_renderer
	{
	public :
		Entity_renderer(const Entity_container& entity_container)
			: render_mesh_id_(alloc_mesh())
			, shader_(graphics::Renderer::instance().allocate_shader_program("basic"))
			, entity_container_(entity_container)
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

				entity_container_.for_each([&projection, &view, this](const Entity&) 
				{
					const glm::mat4 model(1.0f);

					model_view_projection_.set(model*view*projection);
					model_transform_.set(model);

					graphics::Renderer::instance().render_mesh(render_mesh_id_);
				});
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

		const Entity_container& entity_container_;

		Entity_renderer(const Entity_renderer&) = delete;
		Entity_renderer& operator=(const Entity_renderer&) = delete;
	};

	class World_renderer; 
	class Particle_renderer;
	class Gui_renderer;

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

		Entity_container entity_container;
		Entity_renderer entity_renderer(entity_container);

		entity_container.add(Entity());

		for (;;)
		{
			graphics::Renderer::instance().begin_render(); 

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

