#pragma once
#include "my_glm.hpp"

namespace base {

	using Vec2 = glm::vec2; 
	using Vec3 = glm::vec3;
	
	inline decltype(Vec3::x) dot(const Vec3& v0, const Vec3& v1)
	{
		return glm::dot(v0, v1);
	}

	inline Vec3 cross(const Vec3& v0, const Vec3& v1)
	{
		return glm::cross(v0, v1);
	}

	inline Vec3 normal(const Vec3& v0, const Vec3& v1, const Vec3& v2)
	{
		return glm::normalize(glm::cross(v2 - v0, v1 - v0));
	}

	inline Vec3 normalize(const Vec3& v0)
	{
		return glm::normalize(v0);
	}
}
