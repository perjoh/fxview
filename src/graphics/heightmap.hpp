#pragma once
#include "../base/vec.hpp"

namespace graphics {

	class Heightmap { 
	public :
		Heightmap(unsigned width, unsigned height);

	public :
		void set(unsigned x, unsigned y, float value);

	public :
		float sample(float x, float y) const;

	public :
		float internal_sample(unsigned x, unsigned y) const;

	private :
		std::vector<float> map_;
		unsigned width_, height_;
	};

}
