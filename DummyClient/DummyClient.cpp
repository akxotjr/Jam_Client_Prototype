#include "pch.h"
#include "Game.h"
#include "DummyClient.h"
#include "TimeManager.h"

int main()
{
	Game game;
	game.Init();

	double elapsedTime = 0.0;

	while (true)
	{
		TimeManager::Instance().Update();
		elapsedTime += TimeManager::Instance().GetDeltaTime();

		while (elapsedTime >= TICK_INTERVAL_S)
		{
			game.Update();
			game.Render();

			elapsedTime -= TICK_INTERVAL_S;
		}
	}
}
