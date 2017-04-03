#pragma once
#include "Engine Algorithms\Bounds.hpp"

class AABB;
class OBB;
class GameObject;

class Collider
{
public:
	Collider(const glm::vec3& center, const glm::vec3& halfSize);

	void SetCenter(const glm::vec3& center);
	void SetGameObject(GameObject* gameObject);

	const Bounds& GetBounds() const;

	virtual bool IsColliding(Collider* other) = 0;
	virtual bool IsColliding(AABB* other) = 0;
	virtual bool IsColliding(OBB* other) = 0;

	static bool IsColliding(AABB* one, AABB* other);
	static bool IsColliding(AABB* one, OBB* other);
	static bool IsColliding(OBB* one, OBB* other);
protected:
	Bounds m_bounds;			//The bounds defining the position and size of this collider
	GameObject* m_gameObject;	//The game object that owns this collider
private:
	static bool CheckCollisionsOnObjectPlanes(Collider* one, Collider* other, const glm::mat3& onePlaneNormals, const glm::mat3& otherPlaneNormals);
	static bool CheckOverlapOnPlane(Collider* one, Collider* other, const glm::vec3& planeNormal);
};