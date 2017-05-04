#include "heightmap.hpp"

namespace graphics {

	Heightmap::Heightmap(unsigned width, unsigned height)
		: map_(width*height)
		, width_(width)
		, height_(height)
	{
	}

	void Heightmap::set(unsigned x, unsigned y, float value)
	{ 
		inernal_sample(x, y) = value;
	}

	float Heightmap::sample(float x, float y) const
	{
	}

	float& Heightmap::internal_sample(unsigned x, unsigned y) const
	{ 
		return map_[x*widht + y];
	}
}
