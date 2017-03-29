#include "OBB.hpp"

OBB::OBB(const glm::vec3 & halfExtents, const glm::vec3 & center) : Collider(center, halfExtents) {}

bool OBB::IsColliding(Collider * other)
{
	return other->IsColliding(this);
}

bool OBB::IsColliding(AABB * other)
{
	return Collider::IsColliding(other, this);
}

bool OBB::IsColliding(OBB * other)
{
	return Collider::IsColliding(this, other);
}
