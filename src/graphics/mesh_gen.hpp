#pragma once
#include "mesh.hpp"

namespace graphics {
namespace mesh {

    mesh::Triangle_mesh<> generate_smooth_cube(const glm::vec3& size, unsigned level_of_detail = 4);
    mesh::Triangle_mesh<> generate_cube(const glm::vec3& size);

    mesh::Triangle_mesh<> generate_grid(unsigned int width, unsigned int height, float unit_size = 1.0f);

} // namespace shape_gen
} // namespace graphics 