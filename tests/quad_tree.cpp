#include "../src/spatial/quad_tree.hpp"
#include "catch.hpp"

TEST_CASE("Quad_tree")
{
	using Point = glm::ivec2;
	spatial::tree::Quad_tree<Point, 3> tree();
}