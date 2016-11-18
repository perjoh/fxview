#include <iostream>
#include "graphics/window.hpp"


int main()
{
	graphics::open_window(640, 480, "Hello world");

	while (graphics::run_window())
	{ }

	graphics::close_window();

	return 0;
}

