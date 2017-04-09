#include "FreeLookCamera.hpp"
#include "SFML\Window\Keyboard.hpp"
#include "SFML\Window\Mouse.hpp"
#include "mge\core\GameObject.hpp"

FreeLookCamera::FreeLookCamera(const sf::RenderWindow & window) : m_window(window) {}

void FreeLookCamera::SetMoveSpeed(float speed)
{
	m_moveSpeed = speed;
}

void FreeLookCamera::SetRotationSpeed(float speed)
{
	m_rotationSpeed = speed;
}

void FreeLookCamera::update(float deltaTime)
{
	Rotate();
	Move(deltaTime);
}

glm::vec2 FreeLookCamera::GetMousePosition()
{
	const sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);
	return glm::vec2(mousePosition.x, mousePosition.y);
}

bool pressed = false;
void FreeLookCamera::Rotate()
{
	//Once the mouse button is pressed, update the previous mouse position
	if (pressed == false && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		m_previousMousePos = GetMousePosition();
		pressed = true;
	}

	//Only move the camera if the right mouse button is held
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		const glm::vec2 currentMousePosition = GetMousePosition();

		//Calculate the delta mouse movement
		glm::vec2 deltaMousePosition = currentMousePosition - m_previousMousePos;

		//Update the previous position
		m_previousMousePos = currentMousePosition;

		//Convert sf vector to glm vector for math operations
		deltaMousePosition *= m_rotationSpeed;

		//Calculate the new rotation angles
		m_accumulatedXRotation -= deltaMousePosition.y;
		m_accumulatedYRotation -= deltaMousePosition.x;

		//Apply rotation
		_owner->setTransform(
			glm::rotate(glm::radians(m_accumulatedXRotation), glm::vec3(1.0f, 0.0f, 0.0f))
			//glm::rotate(glm::radians(m_accumulatedYRotation), glm::vec3(0.0f, 1.0f, 0.0f))
		);
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) == false)
	{
		pressed = false;
	}
}

void FreeLookCamera::Move(float deltaTime)
{
	glm::vec3 movement = glm::vec3(0.0f);
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		movement.x = -1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		movement.x = 1;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		movement.z = -1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		movement.z = 1;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		movement.y = -1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		movement.y = 1;
	}

	//If no movement input is given, reset the acceleration clock and do nothing
	if (glm::length2(movement) < 0.001f)
	{
		m_accelerationClock.restart();
		return;
	}
	_owner->translate(
		movement * (m_moveSpeed + m_accelerationClock.getElapsedTime().asSeconds() * 5.0f) * deltaTime
	);
}
