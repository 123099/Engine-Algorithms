#include "mge/core/AbstractGame.hpp"
#include "mge/MGEDemo.hpp"

int main()
{
    AbstractGame* game = new MGEDemo();
		game->initialize();
		game->run();
	delete game;

    return 0;
}



