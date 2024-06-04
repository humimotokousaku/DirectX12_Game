#include "GameTime.h"

GameTimer* GameTimer::GetInstance() {
	static GameTimer instance;
	return &instance;
}