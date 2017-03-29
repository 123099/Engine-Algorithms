#include "AABB.hpp"

AABB::AABB(const glm::vec3 & halfExtents, const glm::vec3 & center) : Collider(center, halfExtents) {}

bool AABB::IsColliding(Collider * other)
{
	return other->IsColliding(this);
}

bool AABB::IsColliding(AABB * other)
{
	return Collider::IsColliding(this, other);
}

bool AABB::IsColliding(OBB * other)
{
	return Collider::IsColliding(this, other);
}

