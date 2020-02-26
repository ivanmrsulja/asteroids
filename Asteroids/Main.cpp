#include "AsteroidsGame.h"


int main()
{

	Game game;
	if (game.Construct(SCREEN_WIDTH, SCREEN_HEIGHT, 2, 2))
		game.Start();

	return 0;
}