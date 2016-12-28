#pragma once

namespace graphics 
{

	void create_graphics(	unsigned window_width, 
				unsigned window_height, 
				const char* window_title); 

	void cleanup_graphics();

	void begin_render();
	void end_render();

	bool handle_events();

}
