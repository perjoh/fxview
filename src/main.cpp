#include "graphics/window.hpp"
#include "graphics/mesh.hpp"
#include "graphics/mesh_gen.hpp"
#include "graphics/render.hpp"
#include "graphics/bezier.hpp"
#include "base/task_runner.hpp"
#include <iostream>
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cassert>
#include <vector>
#include <glm/gtx/transform.hpp>

// Purpose: Generate a triangle mesh from a set of curves.
//	Path curve - path along which to place the radius curve.
//	Radius curve - the core shape of the mesh.
//	Scale curve - determines the size of radius along path.
// Rotation curve - determines rotation of radius along path.

/*using graphics::mesh::point;
 
	graphics::curve::bezier_n<point> radius_curve{
		point(0.0f, 0.0f, 1.0f),
		point(1.0f, 0.0f, 1.0f),
		point(1.0f, 0.0f, 0.0f),
		point(1.0f, 0.0f, -1.0f),
		point(0.0f, 0.0f, -1.0f),
		point(-1.0f, 0.0f, -1.0f),
		point(-1.0f, 0.0f, 0.0f),
		point(-1.0f, 0.0f, 1.0f),
		point(0.0f, 0.0f, 1.0f)
	}; 

	graphics::curve::bezier_quadratic<point> test_curve{
		point(0.0f, 0.0f, 1.0f),
		point(1.0f, 0.0f, 1.0f),
		point(1.0f, 0.0f, 0.0f),
		1.0f, 2.0f, 1.0f };

	std::vector<graphics::curve::bezier_quadratic<point>> curve_sequence{
		graphics::curve::generate_circular_sequence({	point(-1.0f, 0.0f, -1.0f), 
														point(1.0f, 0.0f, -1.0f), 
														//point(1.0f, 0.0f, 1.0f), 
														point(-1.0f, 0.0f, 1.0f)}, 2.5f) 
	};*/

	/*unsigned allocate_generated_mesh()
	{
		const graphics::curve::quadratic_sequence<point> radius_curve(
			graphics::curve::generate_circular_sequence({
				point(-1.0f, 0.0f, -1.0f),
				point(1.0f, 0.0f, -1.0f),
				point(1.0f, 0.0f, 1.0f),
				point(-1.0f, 0.0f, 1.0f)
			}, 2.5f));

		const graphics::curve::linear_curve<point> path_curve(point(0.0f, 0.0f, 0.0f),
			point(0.0f, 7.5f, 0.0f));

		const point p0{0.0f, 0.0f, 0.0f};
		const point p1{3.0f, 0.0f, 0.0f};
		const point p2{3.0f, 1.0f, 0.0f};
		const point p3{0.0f, 1.0f, 0.0f};

		//graphics::curve::bezier_quadratic<point> c0{p0, p1, p2};
		//graphics::curve::bezier_quadratic<point> c1{p2, p3, p4};
		//graphics::curve::bezier_quadratic<point> c2{p4, p5, p6};

		graphics::curve::bezier_weighted<point> scale_curve{ {p0, p1, p2, p3}, {1.0f, 1.0f, 1.0f, 1.0f} };

		//const graphics::curve::quadratic_sequence<point> scale_curve{ c0, c1, c2 };

		//const graphics::curve::linear_curve<point> scale_curve(point(1.0f), point(1.0f));


		const unsigned num_segments = 32;
		const unsigned segment_size = 16;

		auto shape = graphics::mesh::shaper::generate_shape(radius_curve,
															path_curve,
															scale_curve,
															num_segments,
															segment_size);

		std::vector<graphics::data::Vertex> vertices(shape.size());
		for (unsigned i = 0; i < shape.size(); ++i)
		{
			vertices[i].position = shape[i];
			vertices[i].color = glm::vec3(1.0f);
			vertices[i].normal = glm::vec3(0.0f, 1.0f, 0.0f);
		}

		auto triangles = graphics::mesh::shaper::triangulate_shape(	num_segments, 
																	segment_size);

		graphics::data::calc_vertex_normals(&vertices[0], 
											vertices.size(), 
											triangles);

		return graphics::render::alloc_indexed_triangles(	&vertices[0], 
														vertices.size(), 
														&triangles[0].v0, 
														triangles.size()*3);
	}*/


	unsigned mesh_id = graphics::render::Mesh_renderer::invalid_mesh_id;

	void setup_my_render()
	{
		//mesh_id = graphics::mesh::generate_cube(glm::vec3(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		auto cube = graphics::mesh::generate_cube(glm::vec3(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mesh_id = graphics::render::Renderer::instance().mesh_renderer().allocate_mesh(cube);
	}

	void my_render()
	{
		assert(mesh_id != graphics::render::Mesh_renderer::invalid_mesh_id);

		using namespace graphics::render;

		Mesh_renderer& mesh_renderer = Renderer::instance().mesh_renderer();

		const glm::mat4 persp{ glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f) };
		mesh_renderer.set_projection_transform(persp);

		const glm::mat4 view = glm::lookAt(glm::vec3(5, 15, 10),
										   glm::vec3(0, 0, 0),
										   glm::vec3(0, 1, 0));

		mesh_renderer.set_view_transform(view);

		auto transform = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mesh_renderer.render(mesh_id, glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	class Anim_task
	{
	public:
		Anim_task()
		{
			using namespace base;
			Task_runner::instance().add_task(Task_runner::Task_delegate::construct<Anim_task, &Anim_task::Update>(this));
		}

	public :
		unsigned Update(float)
		{
			return base::Task_runner::task_ok;
		}

	private :
	};

	class Player_anim
	{
	public :
	private : 
	};


int main()
{
	try
	{
		graphics::create_graphics(800, 600, "Hello world");

		setup_my_render();

		for (;;)
		{
			graphics::begin_render(); 

			my_render(); 
			const bool keep_going = graphics::handle_events();
			base::Task_runner::instance().run();

			graphics::end_render();

			if (!keep_going)
				break;
		}

		graphics::cleanup_graphics();
	}
	catch (const std::exception& e)
	{
		std::cerr << "EXCEPTION CAUGHT: " << e.what() << std::endl;
	}

	return 0;
}

