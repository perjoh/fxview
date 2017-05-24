#pragma once
#include "mesh.hpp"

namespace kvant {
namespace graphics {

    Triangle_mesh<> generate_smooth_cube(const glm::vec3& size, unsigned level_of_detail = 4);
    Triangle_mesh<> generate_cube(const glm::vec3& size);

    Triangle_mesh<> generate_checkerboard(unsigned int width, unsigned int height, float unit_size = 1.0f);

} // namespace shape_gen
} // namespace graphics 
