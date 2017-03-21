#pragma once
#include "AbstractBehaviour.hpp"
#include <SFML\System\Vector2.hpp>
#include <SFML\System\Clock.hpp>
#include "SFML\Graphics\RenderWindow.hpp"
#include "glm.hpp"

class FreeLookCamera final : public AbstractBehaviour
{
public:
	FreeLookCamera(const sf::RenderWindow& window);

	void SetMoveSpeed(float speed);
	void SetRotationSpeed(float speed);

	void update(float deltaTime) override final;
private:
	const sf::RenderWindow& m_window;

	float m_moveSpeed;
	float m_rotationSpeed;

	glm::vec2 m_previousMousePos;
	float m_accumulatedXRotation;
	float m_accumulatedYRotation;

	sf::Clock m_accelerationClock;

	glm::vec2 GetMousePosition();

	void Rotate();
	void Move(float deltaTime);
};

