#pragma once

class Collider;

class CollisionPair final
{
public:
	CollisionPair(Collider* one, Collider* other) : m_one(one), m_other(other) {}

	bool operator==(const CollisionPair& other)
	{
		return (m_one == other.m_one || m_one == other.m_other) && (m_other == other.m_one || m_other == other.m_other);
	}

	bool operator !=(const CollisionPair& other)
	{
		return !(*this == other);
	}
private:
	Collider* m_one;
	Collider* m_other;
};