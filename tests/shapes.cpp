#include "../src/spatial/shapes.hpp"
#include "catch.hpp"

using namespace spatial::shapes;

TEST_CASE("Circle basic tests")
{ 
	using Point = glm::ivec2;
	Point center{0, 0};
	Circle<Point> c(center, 1);
	REQUIRE(c.center == center);
	REQUIRE(c.radius == 1);
}

TEST_CASE("Rectangle basic tests")
{
	using Point = glm::ivec2;
	Point min{0, 0};
	Point max{10, 10};
	Rectangle<Point> rect(Rectangle<Point>::construct_minmax(max, min)); // Reversed arguments
	REQUIRE(rect.min == min);
	REQUIRE(rect.max == max); 
	REQUIRE(rect.width() == 10);
	REQUIRE(rect.height() == 10);
	REQUIRE(rect.center().x == 5);
	REQUIRE(rect.center().y == 5);
	REQUIRE(rect.area() == 100);

	SECTION("Splitting")
	{
		auto split = rect.split();
		REQUIRE(split[0].width() == 5);
		REQUIRE(split[0].height() == 5);
		REQUIRE(split[1].width() == 5);
		REQUIRE(split[1].height() == 5);
		REQUIRE(split[2].width() == 5);
		REQUIRE(split[2].height() == 5);
		REQUIRE(split[3].width() == 5);
		REQUIRE(split[3].height() == 5);

		auto split_horiz = rect.split(0);
		REQUIRE(split_horiz[0].width() == 5);
		REQUIRE(split_horiz[0].height() == 10);
		REQUIRE(split_horiz[1].width() == 5);
		REQUIRE(split_horiz[1].height() == 10);
	} 

	SECTION("Collision detection")
	{ 
		Circle<Point> circle_inside{ Point(5,5), 2 };
		Circle<Point> circle_outside{ Point(-3,5), 2 };
		Circle<Point> circle_cross{ Point(11,11), 4 };
		REQUIRE(rect.contains(circle_inside));
		REQUIRE(!rect.contains(circle_outside));
		REQUIRE(!rect.contains(circle_cross));

		Rectangle<Point> rect_inside{Point(1,1), Point(9,9)};
		Rectangle<Point> rect_outside{Point(11, 0), Point(15, 7)};
		Rectangle<Point> rect_cross{Point(-4, 1), Point(6, 8)};
		REQUIRE(rect.contains(rect_inside));
		REQUIRE(!rect.contains(rect_outside));
		REQUIRE(!rect.contains(rect_cross));
	}
}
