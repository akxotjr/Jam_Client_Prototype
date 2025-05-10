#include "pch.h"
#include "Game.h"
#include "DummyClient.h"
#include "TimeManager.h"

bool running = true;

int main()
{
	Game game;
	game.Init();

	double elapsedTime = 0.0;

	while (running)
	{
		TimeManager::Instance().Update();
		elapsedTime += TimeManager::Instance().GetDeltaTime();

		while (elapsedTime >= LOGIC_TICK_INTERVAL)
		{
			game.Update();
			elapsedTime -= LOGIC_TICK_INTERVAL;
		}

		game.Render();
	}
}
