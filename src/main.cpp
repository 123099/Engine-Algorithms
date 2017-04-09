#include "mge/core/AbstractGame.hpp"
#include "mge/MGEDemo.hpp"
#include "Engine Algorithms\Collider.hpp"
#include "Engine Algorithms\AABB.hpp"
#include "Engine Algorithms\OBB.hpp"
#include "Engine Algorithms\CollisionPair.hpp"

int main()
{
    AbstractGame* game = new MGEDemo();
		game->initialize();
		game->run();
	delete game;

    return 0;
}



