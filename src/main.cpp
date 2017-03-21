#include "mge/core/AbstractGame.hpp"
#include "mge/MGEDemo.hpp"

//#define FINAL_DEMO

int main()
{
    AbstractGame* game = new MGEDemo();
		game->initialize();
		game->run();
	delete game;

    return 0;
}



