#ifndef MGEDEMO_H
#define MGEDEMO_H

#include <mge/core/AbstractGame.hpp>
#include "Engine Algorithms\OcTreeImpl.hpp"
#include "Engine Algorithms\CollisionPair.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

class DebugHud;
class Collider;

class MGEDemo: public AbstractGame
{
public:
	MGEDemo();
	virtual ~MGEDemo();

    virtual void initialize();

protected:
    virtual void _initializeScene();

	void _update(float deltaTime) override;
	//override render to render the hud as well.
	virtual void _render();

private:
	DebugHud* m_hud;
	OcTreeImpl* m_octree;
	std::vector<GameObject*> m_collisionsRetrieved;
	std::unordered_map<Collider*, std::unordered_set<Collider*>> m_processedCollisionPairs;

	void Test(unsigned objectCount);
	void UpdateOcTree();
	void ProcessCollisions();
    void _updateHud();

	MGEDemo(const MGEDemo&) = delete;
	MGEDemo& operator=(const MGEDemo&) = delete;
};

#endif // MGEDEMO_H
