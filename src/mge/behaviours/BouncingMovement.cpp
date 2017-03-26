#include "BouncingMovement.hpp"
#include "mge\core\GameObject.hpp"
#include <random>

BouncingMovement::BouncingMovement(float range) : m_range(range)
{
	m_movementDirection = (glm::vec3((float)std::rand(), (float)std::rand(), (float)std::rand()) / glm::vec3(RAND_MAX) - glm::vec3(0.5f)) * 2.0f;
}

void BouncingMovement::update(float deltaTime)
{
	const glm::vec3& position = _owner->getLocalPosition();

	if (position.x > m_range || position.x < -m_range)
		m_movementDirection.x *= -1;

	if (position.y > m_range || position.y < -m_range)
		m_movementDirection.y *= -1;

	if (position.z > m_range || position.z < -m_range)
		m_movementDirection.z *= -1;
	
	_owner->translate(m_movementDirection * deltaTime * 5.0f);
}
