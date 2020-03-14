#include "AsteroidsGame.h"


int main()
{
	srand(time(NULL));
	Game game;
	if (game.Construct(SCREEN_WIDTH, SCREEN_HEIGHT, 2, 2))
		game.Start();

	return 0;
}