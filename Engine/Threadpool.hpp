#pragma once
#include "../Shared/D2Shared.hpp"

// Threadpool.cpp
namespace Threadpool
{
	void WaitUntilCompletion();
	void SpawnJob(D2AsyncTask job, void* pData);
	void Init();
	void Shutdown();
}