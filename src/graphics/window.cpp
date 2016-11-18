#include <SDL.h>
#include <GL/glew.h>
#include <stdexcept>
#include "window.hpp"

namespace graphics {

	SDL_Window* g_window = nullptr; 
	SDL_GLContext g_glcontext = nullptr;


	void open_window(	unsigned window_width, 
						unsigned window_height, 
						const char* window_title) 
	{ 
		g_window = ::SDL_CreateWindow(	window_title,
										SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										window_width, window_height,
										SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

		if (nullptr == g_window) 
		{
			throw std::runtime_error("Failed to create window.");
		}

		g_glcontext = ::SDL_GL_CreateContext(g_window);
		if (nullptr == g_glcontext) 
		{
			throw std::runtime_error("Failed to create OpenGL context.");
		}
	}

	void clear_buffers()
	{
        glClearColor(0.0, 0.0, 0.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}

	bool run_window() 
	{ 
		clear_buffers();

		::SDL_Event e;
		while (::SDL_PollEvent(&e)) 
		{ 
			switch (e.type) 
			{ 
				case SDL_KEYUP:
					if (e.key.keysym.sym == SDLK_ESCAPE)
						return false;
					break;
				case SDL_QUIT:
					return false;
			}
		}

		::SDL_GL_SwapWindow(g_window);

		return true; 
	}


	void close_window() 
	{ 
		if (g_window) 
		{
			::SDL_DestroyWindow(g_window);
		}
	}

}
