#include "Bounds.hpp"

Bounds::Bounds(const glm::vec3 & center, const glm::vec3 & halfExtents) : m_center(center), m_halfExtents(halfExtents) {}

const glm::vec3 & Bounds::GetCenter() const
{
	return m_center;
}

const glm::vec3 & Bounds::GetExtents() const
{
	return m_halfExtents;
}
