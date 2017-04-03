#include "Collider.hpp"
#include "AABB.hpp"
#include "OBB.hpp"
#include "mge\core\GameObject.hpp"
#include <iostream>

Collider::Collider(const glm::vec3 & center, const glm::vec3 & halfSize) : m_bounds(center, halfSize) {}

void Collider::SetCenter(const glm::vec3 & center)
{
	m_bounds.SetCenter(center);
}

void Collider::SetGameObject(GameObject * gameObject)
{
	m_gameObject = gameObject;
	SetCenter(m_gameObject->getLocalPosition());
}

const Bounds & Collider::GetBounds() const
{
	return m_bounds;
}

bool Collider::IsColliding(AABB * one, AABB * other)
{
	//Get the minimum and the maximum of both AABBs
	const glm::vec3 oneMin = one->GetBounds().GetMinimum();
	const glm::vec3 oneMax = one->GetBounds().GetMaximum();
	const glm::vec3 otherMin = other->GetBounds().GetMinimum();
	const glm::vec3 otherMax = other->GetBounds().GetMaximum();

	//Check for overlaps on all 3 axes
	return (oneMin.x <= otherMax.x && oneMax.x >= otherMin.x) &&
		   (oneMin.y <= otherMax.y && oneMax.y >= otherMin.y) &&
		   (oneMin.z <= otherMax.z && oneMax.z >= otherMin.z);
}

bool Collider::IsColliding(AABB * one, OBB * other)
{
	return CheckCollisionsOnObjectPlanes(one, other, glm::mat3(), other->m_gameObject->getTransform());
}

bool Collider::IsColliding(OBB * one, OBB * other)
{
	return CheckCollisionsOnObjectPlanes(one, other, one->m_gameObject->getTransform(), other->m_gameObject->getTransform());
}

bool Collider::CheckCollisionsOnObjectPlanes(Collider* one, Collider* other, const glm::mat3 & onePlaneNormals, const glm::mat3 & otherPlaneNormals)
{
	//Check for overlap on the plane axes of each of the two objects
	for (unsigned i = 0; i < 3; ++i)
	{
		if (CheckOverlapOnPlane(one, other, onePlaneNormals[i]) == false || CheckOverlapOnPlane(one, other, otherPlaneNormals[i]) == false)
		{
			return false;
		}
	}

	//Check for overlap on all the possible combinations of the axes of the two objects
	for (unsigned i = 0; i < 3; ++i)
	{
		const glm::vec3& oneAxis = onePlaneNormals[i];
		for (unsigned j = 0; j < 3; ++j)
		{
			const glm::vec3 combinedPlaneAxis = glm::cross(oneAxis, otherPlaneNormals[j]);

			//Only check for overlap if the two axes of the two objects are not similar, meaning that the normal of the plane is longer than 0.1 units
			if (glm::length2(combinedPlaneAxis) >= 0.01f)
			{
				if (CheckOverlapOnPlane(one, other, combinedPlaneAxis) == false)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool Collider::CheckOverlapOnPlane(Collider* one, Collider* other, const glm::vec3 & planeNormal)
{
	//Get distance between the 2 centers of the colliders projected on the plane normal
	const float centerDistance = std::abs(glm::dot(other->GetBounds().GetCenter() - one->GetBounds().GetCenter(), planeNormal));

	//Calculate the extents of the bounds, taking into consideration the orientation of the colliders
	const glm::mat4 oneExtents = glm::scale(one->m_gameObject->getTransform(), one->GetBounds().GetExtents());
	const glm::mat4 otherExtents = glm::scale(other->m_gameObject->getTransform(), other->GetBounds().GetExtents());

	//Get the distance between the center of the objects and its maximum point projected on the plane normal
	const float radiusOne =
		std::abs(glm::dot(glm::vec3(oneExtents[0]), planeNormal)) +
		std::abs(glm::dot(glm::vec3(oneExtents[1]), planeNormal)) +
		std::abs(glm::dot(glm::vec3(oneExtents[2]), planeNormal));
		//std::abs(glm::dot(glm::vec3(oneExtents[0]) + glm::vec3(oneExtents[1]) + glm::vec3(oneExtents[2]), planeNormal));
	const float radiusTwo = 
		std::abs(glm::dot(glm::vec3(otherExtents[0]), planeNormal)) +
		std::abs(glm::dot(glm::vec3(otherExtents[1]), planeNormal)) +
		std::abs(glm::dot(glm::vec3(otherExtents[2]), planeNormal));
		//std::abs(glm::dot(glm::vec3(otherExtents[0]) + glm::vec3(otherExtents[1]) + glm::vec3(otherExtents[2]), planeNormal));

	return radiusOne + radiusTwo > centerDistance;
}
