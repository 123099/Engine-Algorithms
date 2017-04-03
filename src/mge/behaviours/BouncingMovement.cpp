#include "BouncingMovement.hpp"
#include "mge\core\GameObject.hpp"
#include "Engine Algorithms\Collider.hpp"
#include <random>

BouncingMovement::BouncingMovement(float range) : m_range(range)
{
	m_movementDirection = (glm::vec3((float)std::rand(), (float)std::rand(), (float)std::rand()) / glm::vec3(RAND_MAX) - glm::vec3(0.5f)) * 2.0f;
}

void BouncingMovement::update(float deltaTime)
{
	glm::vec3 position = _owner->getLocalPosition();

	if (position.x > m_range)
	{
		m_movementDirection.x = -1;
		position.x = m_range;
	}
	else if (position.x < -m_range)
	{
		m_movementDirection.x = 1;
		position.x = -m_range;
	}

	if (position.y > m_range)
	{
		m_movementDirection.y = -1;
		position.y = m_range;
	}
	else if (position.y < -m_range)
	{
		m_movementDirection.y = 1;
		position.y = -m_range;
	}

	if (position.z > m_range)
	{
		m_movementDirection.z = -1;
		position.z = m_range;
	}
	else if (position.z < -m_range)
	{
		m_movementDirection.z = 1;
		position.z = -m_range;
	}
	
	if (position != _owner->getLocalPosition())
	{
		_owner->setLocalPosition(position);
	}

	_owner->rotate(deltaTime, glm::vec3(1, 0, 0));
	_owner->translate(m_movementDirection * deltaTime * 5.0f);
}

void BouncingMovement::OnCollision(Collider * other)
{
	m_movementDirection *= -2;
}
