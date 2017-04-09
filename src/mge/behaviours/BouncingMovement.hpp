#pragma once
#include "AbstractBehaviour.hpp"
#include "glm.hpp"

class BouncingMovement final : public AbstractBehaviour
{
public:
	BouncingMovement(float range);

	void update(float deltaTime) override final;
	void OnCollision(Collider* other) override final;
private:
	float m_range;
	glm::vec3 m_movementDirection;
};