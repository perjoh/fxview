#pragma once
#include <glm/glm.hpp>

namespace spatial2d {
namespace shapes {

	typedef glm::vec2 vec;

	//
	inline bool test_point_circle(	const vec& point,
									const vec& circle_center,
									float radius)
	{
		const auto v = circle_center - point;
		return glm::dot(v, v) <= radius*radius;
	}

	//
	inline bool test_circle_circle( const vec& center_a, float radius_a,
									const vec& center_b, float radius_b)
	{
		return test_point_circle(center_a, center_b, radius_a + radius_b);
	}

	//
	inline bool test_point_rect(const vec& point,
								const vec& rect_center,
								const vec& rect_dim)
	{
		return	point.x < (rect_center.x + rect_dim.x) &&
				(rect_center.x - rect_dim.x) <= point.x &&
				point.y < (rect_center.y + rect_dim.y) &&
				(rect_center.y - rect_dim.y) <= point.y;
	}

	template <typename value_type>
	inline value_type limit(value_type v, value_type min, value_type max)
	{
		if (v < min)
			return min;
		else if (max < v)
			return max;
		return v;
	}

	//
	inline vec nearest_point_rect(	const vec& point,
									const vec& rect_center,
									const vec& rect_dim)
	{
		return vec( limit(point.x, rect_center.x - rect_dim.x, rect_center.x + rect_dim.x),
					limit(point.y, rect_center.y - rect_dim.y, rect_center.y + rect_dim.y));
	}

	//
	inline bool test_circle_rect(	const vec& circle_center,
									float radius,
									const vec& rect_center,
									const vec& rect_dim)
	{
		auto nearest = nearest_point_rect(circle_center, rect_center, rect_dim);
		return test_point_circle(nearest, circle_center, radius);vec
	}


    template <typename T>
	struct circle
	{
		circle(const vec& c, float r)
			: center(c)
			, radius(r)
		{ }

		vec min() const
		{
			return vec(center.x - radius, center.y - radius);
		}

		vec max() const
		{
			return vec(center.x + radius, center.y + radius);
		}

		vec center;
		T radius;
	};


	struct rect
	{
		rect(	const vec& min_,
				const vec& max_)
			: min(min_)
			, max(max_)
		{ }

		static rect construct_minmax(	const vec& a,
										const vec& b)
		{
			const vec min_( std::min(a[0], b[0]),
							std::min(a[1], b[1]),
							std::min(a[2], b[2]));

			const vec max_( std::max(a[0], b[0],
							std::max(a[1], b[1],
							std::max(a[2], b[2]));

			return rect(min_, max_);
		}

		std::array<rect, 2> split(unsigned axis) const
		{
			const vec c{center()};
			vec a{max};
			a[axis] = c[axis];
			vec b{min};
			b[axis] = c[axis];
			return {rect(min, a), rect(b, max)};
		}

		std::array<rect, 4> split() const
		{
			const vec c{center()};

			return { 	rect(vec(min[0], c[1]), vec(c[0], max[1])),
						rect(c, max)),
						rect(min, c)),
						rect(vec(c[0], min[1]), vec(max[0], c[1])))};
		}

		float width() const
		{
			return max[0] - min[0];
		}

		float height() const
		{
			return max[1] - min[1];
		}

		float area() const
		{
			return widht()*height();
		}

		vec center() const
		{
			return (max - min)*0.5 + min;
		}

		template <typename shape_type>
		bool contains(const shape_type& shape) const
		{
			const rect tmp{shape.min(), shape.max()};
			return contains(tmp);
		}

		bool contains(const rect& other) const
		{
			return 	other.max()[0] < max[0] &&
					other.max()[1] < max[1] &&
					min()[0] <= other.min[0] &&
					min()[1] <= other.min[1];
		}

		vec min;
		vec max;
	};

}}
