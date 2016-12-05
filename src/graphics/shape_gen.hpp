#pragma once
#include "bezier_curve.hpp"

namespace graphics {

	// 
	template <typename point_type>
	class path_volume
	{
	public :
	private :
		typedef bezier_curve<point_type> curve_type;
		curve_type spine_;
		curve_type spine_scale_;
		curve_type radial_;
	};

}
