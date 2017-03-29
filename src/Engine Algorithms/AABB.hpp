#pragma once
#include "Collider.hpp"
#include "Bounds.hpp"

class AABB final : public Collider
{
public:
	AABB(const glm::vec3& halfExtents, const glm::vec3& center = glm::vec3(0.0f));

	bool IsColliding(Collider* other) override final;
	bool IsColliding(AABB* other) override final;
	bool IsColliding(OBB* other) override final;
};