#pragma once

namespace graphics {

	// 1. Start with arbitrary point, A. 
	// 2. Remove A from set.
	// 3. Find point closest to A, B. 
	// 4. Remove B from set.
	// 5. Find point closest to B, C.
	// 6. Remove C from set.
	// 7. A, B, C forms a triangle. Add it to the result.
	// 8. If there is more points in the set, find point closest to C.
	// 9. 

	using Point = ;
	using Points = std::vector<Point>;

	std::pair<size_t, size_t> furthest_points(const Points& points)
	{
		size_t a = 0;
		size_t b = 0;
		float max_dist = -1.0f;
		float tmp_dist = 0.0f;
		std::pair<size_t, size_t> result;

		while (max_dist < tmp_dist)
		{ 
			max_dist = tmp_dist;
			tmp_dist = 0.0f;

			result.first = a;
			result.second = b;

			a = b;

			for (size_t i = 0; i < points.size(); ++i)
			{
				if (i != a)
				{
					const float dist = distance(points[a], points[i]);
					if (tmp_dist < dist)
					{
						tmp_dist = dist;
						b = i;
					}
				}
			} 

		}

		return result;
	}

}
