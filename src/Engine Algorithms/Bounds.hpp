#pragma once
#include "glm.hpp"

class Bounds
{
public:
	Bounds(const glm::vec3& center, const glm::vec3& halfExtents);

	const glm::vec3& GetCenter() const;
	const glm::vec3& GetExtents() const;

private:
	glm::vec3 m_center;
	glm::vec3 m_halfExtents;
};