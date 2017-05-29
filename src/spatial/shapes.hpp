#pragma once
#include <glm/glm.hpp>
#include <array>

#undef min
#undef max

namespace kvant {
namespace spatial {

	using Point = glm::vec2;

	//
	inline bool test_point_circle(	const Point& point,
									const Point& circle_center,
									float radius)
	{
		const auto v = circle_center - point;
		return glm::dot(v, v) <= radius*radius;
	}

	//
	inline bool test_circle_circle( const Point& center_a, float radius_a,
									const Point& center_b, float radius_b)
	{
		return test_point_circle(center_a, center_b, radius_a + radius_b);
	}

	//
	inline bool test_point_rect(const Point& point,
								const Point& rect_center,
								const Point& rect_dim)
	{
		return	point.x < (rect_center.x + rect_dim.x) &&
				(rect_center.x - rect_dim.x) <= point.x &&
				point.y < (rect_center.y + rect_dim.y) &&
				(rect_center.y - rect_dim.y) <= point.y;
	}

	template <typename Value>
	inline Value limit(Value v, Value min, Value max)
	{
		if (v < min)
			return min;
		else if (max < v)
			return max;
		return v;
	}

	//
	inline Point nearest_point_rect(const Point& point,
									const Point& rect_center,
									const Point& rect_dim)
	{
		return Point( 	limit(point.x, rect_center.x - rect_dim.x, rect_center.x + rect_dim.x),
						limit(point.y, rect_center.y - rect_dim.y, rect_center.y + rect_dim.y));
	}

	//
	inline bool test_circle_rect(	const Point& circle_center,
									float radius,
									const Point& rect_center,
									const Point& rect_dim)
	{
		auto nearest = nearest_point_rect(circle_center, rect_center, rect_dim);
		return test_point_circle(nearest, circle_center, radius);
	}


	template <typename Point = glm::vec2>
	struct Circle
	{
		using Float = decltype(Point::x);

		Circle(const Point& c, Float r)
			: center(c)
			, radius(r)
		{ }

		Point min() const
		{
			return Point(center.x - radius, center.y - radius);
		}

		Point max() const
		{
			return Point(center.x + radius, center.y + radius);
		}

		Point center;
		Float radius;
	};


	template <typename Point = glm::vec2>
	struct Rectangle
	{
		using Float = decltype(Point::x);

		Rectangle(	const Point& min_,
					const Point& max_)
			: min(min_)
			, max(max_)
		{ }

		static Rectangle construct_minmax(	const Point& a,
											const Point& b)
		{
			const Point min_(std::min(a[0], b[0]),
							 std::min(a[1], b[1]));

			const Point max_(std::max(a[0], b[0]),
							  std::max(a[1], b[1]));

			return Rectangle(min_, max_);
		}

		std::array<Rectangle, 2> split(unsigned axis) const
		{
			const Point c(center());
			Point a(max);
			a[axis] = c[axis];
			Point b(min);
			b[axis] = c[axis];
			return {Rectangle(min, a), Rectangle(b, max)};
		}

		std::array<Rectangle, 4> split() const
		{
			const Point c(center());

			return { 	Rectangle(Point(min[0], c[1]), Point(c[0], max[1])),
						Rectangle(c, max),
						Rectangle(min, c),
						Rectangle(Point(c[0], min[1]), Point(max[0], c[1]))};
		}

		Float width() const
		{
			return max[0] - min[0];
		}

		Float height() const
		{
			return max[1] - min[1];
		}

		Float area() const
		{
			return width()*height();
		}

		Point center() const
		{
			return (max - min)/Point(2) + min;
		}

		template <typename Shape>
		bool contains(const Shape& shape) const
		{
			const Rectangle tmp{shape.min(), shape.max()};
			return contains(tmp);
		}

		bool contains(const Rectangle& other) const
		{
			return 	other.max[0] < max[0] &&
					other.max[1] < max[1] &&
					min[0] <= other.min[0] &&
					min[1] <= other.min[1];
		}

		Point min;
		Point max;
	};

}}
