#include <iostream>
#include "graphics/window.hpp"
#include "graphics/mesh.hpp"
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


void my_render()
{
	static unsigned mesh_id = graphics::mesh::invalid_mesh_id;

	if (mesh_id == graphics::mesh::invalid_mesh_id)
	{
		std::array<graphics::mesh::vertex, 3> vertices{
			graphics::mesh::vertex{glm::vec3{-1.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0, 0}}, 
			graphics::mesh::vertex{glm::vec3{1.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0, 0}}, 
			graphics::mesh::vertex{glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0, 0}}}; 

		mesh_id = graphics::mesh::alloc_triangles(&vertices[0], 3);
	}

	graphics::mesh::set_projection_transform(glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f));

	const glm::mat4 view = glm::lookAt(glm::vec3(0, 1, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
	graphics::mesh::set_view_transform(view);

	graphics::mesh::draw(mesh_id, glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}


int main()
{
	try
	{
		graphics::create_graphics(640, 480, "Hello world");

		for (;;)
		{
			graphics::begin_render(); 
			my_render(); 
			const bool keep_going = graphics::handle_events();
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

