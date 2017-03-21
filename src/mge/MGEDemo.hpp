#ifndef MGEDEMO_H
#define MGEDEMO_H

#include <mge/core/AbstractGame.hpp>
#include "Engine Algorithms\OcTreeImpl.hpp"

class DebugHud;

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

	void Test(unsigned objectCount);
	void UpdateOcTree();
    void _updateHud();

	MGEDemo(const MGEDemo&) = delete;
	MGEDemo& operator=(const MGEDemo&) = delete;
};

#endif // MGEDEMO_H
