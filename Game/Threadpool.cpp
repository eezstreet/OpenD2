#include "Diablo2.hpp"

#define	THREADPOOL_SIZE	10
#define MAX_THREAD_JOBS	100

#define THREADPOOL_SLEEP_MIN	2
#define THREADPOOL_SLEEP_MAX	10

struct D2ThreadTask
{
	D2AsyncTask task;
	void* pData;
	D2ThreadTask* pBehind;	// the element that is "behind" this one in line
};

static SDL_Thread* gpaThreadPool[THREADPOOL_SIZE]{ 0 };
static bool gbKillThreads = false;

static D2ThreadTask* gpJobQueueHead = nullptr;
static D2ThreadTask* gpJobQueueTail = nullptr;
static SDL_mutex* gpJobQueueMutex = nullptr;
static SDL_semaphore* gpQueueSizeSemaphore = nullptr;

/*
 *	Waits until all of the jobs in the queue have been completed.
 *	@author	eezstreet
 */
void T_WaitUntilCompletion()
{
	while (SDL_SemValue(gpQueueSizeSemaphore) != 0)
	{
		SDL_Delay(2);
	}
}

/*
 *	Wait until a single job on the queue has been completed

/*
 *	Pop a job off of the job queue.
 *	@author	eezstreet
 */
static void T_PopJob()
{
	if (gpJobQueueHead == nullptr)
	{	// no jobs?
		return;
	}

	// Lock the head
	SDL_LockMutex(gpJobQueueMutex);

	// Pop the head off
	D2ThreadTask* pCurrent = gpJobQueueHead;
	if (gpJobQueueTail == gpJobQueueHead)
	{
		gpJobQueueTail = nullptr;
	}
	gpJobQueueHead = gpJobQueueHead->pBehind;


	// Unlock the head (and the tail too, if we need to)
	SDL_UnlockMutex(gpJobQueueMutex);

	// Actually do the job
	pCurrent->task(pCurrent->pData);
	
	// Free it
	free(pCurrent);
}

/*
 *	Push a job onto the job queue.
 *	@author	eezstreet
 */
void T_SpawnJob(D2AsyncTask job, void* pData)
{
	// Allocate a thread task
	D2ThreadTask* pCurrent = (D2ThreadTask*)malloc(sizeof(D2ThreadTask));
	pCurrent->task = job;
	pCurrent->pData = pData;
	pCurrent->pBehind = nullptr;

	// Lock the queue mutex
	SDL_LockMutex(gpJobQueueMutex);

	// If there's nothing on the queue, we set the element to be the current.
	// Otherwise, we adjust the tail.
	if (gpJobQueueTail == nullptr)
	{
		gpJobQueueHead = gpJobQueueTail = pCurrent;
	}
	else
	{
		gpJobQueueTail->pBehind = pCurrent;
		gpJobQueueTail = pCurrent;
	}

	// Unlock the queue mutex (so something else can modify it)
	SDL_UnlockMutex(gpJobQueueMutex);

	// Lastly, increment the queue size semaphore
	SDL_SemPost(gpQueueSizeSemaphore);
}

/*
 *	Spawn one of these jobs to kill a worker thread.
 *	@author	eezstreet
 */
static void T_WorkerDie(void* pData)
{
	exit(0);
}

/*
 *	Every tick, the threads in the threadpool will run this function.
 *	@author	eezstreet
 */
static int T_Worker(void* notUsed)
{
	while (!gbKillThreads)
	{
		// wait until the queue length semaphore is greater than 0
		SDL_SemWait(gpQueueSizeSemaphore);

		// take the most recent task and do it
		T_PopJob();
	}

	return 0; // we don't really care about what is returned
}

/*
 *	Initiate the threadpools
 *	@author	eezstreet
 */
void T_Init()
{
	char threadName[32];

	// Create the two mutexes and semaphore associated with the job queue.
	gpJobQueueMutex = SDL_CreateMutex();
	gpQueueSizeSemaphore = SDL_CreateSemaphore(0);

	for (int i = 0; i < THREADPOOL_SIZE; i++)
	{
		snprintf(threadName, 32, "_worker%d", i);
		gpaThreadPool[i] = SDL_CreateThread(T_Worker, threadName, nullptr);
	}
}

/*
 *	Delete the threadpools
 *	@author	eezstreet
 */
void T_Shutdown()
{
	// In global memory, signify that the threads need to die
	gbKillThreads = true;

	// Detach them and watch the magic happen
	for (int i = 0; i < THREADPOOL_SIZE; i++)
	{
		T_SpawnJob(T_WorkerDie, 0);
		SDL_DetachThread(gpaThreadPool[i]);
	}

	// Delete the mutexes and the semaphore
	SDL_DestroySemaphore(gpQueueSizeSemaphore);
	SDL_DestroyMutex(gpJobQueueMutex);
}