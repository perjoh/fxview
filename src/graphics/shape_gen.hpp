#pragma once
#include "mesh.hpp"

namespace graphics { 
namespace shape_gen {

	Triangle_mesh<> smooth_cube(const glm::vec3& size, unsigned level_of_detail = 4); 

}
}
