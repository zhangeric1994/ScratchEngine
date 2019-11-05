#include "JobSystem.h"


ScratchEngine::Job::Job(function<void()> f) : f(f), dependency(0), d(0), next(nullptr)
{
}

ScratchEngine::Job::Job(function<void()> f, Job* next) : f(f), dependency(0), d(0), next(next)
{
	++next->dependency;
}


ScratchEngine::WorkerThread::WorkerThread(u32 capacity) : jobQueue(capacity), runningThread(nullptr), isActive(false)
{
}

void ScratchEngine::WorkerThread::Work()
{
	while (isActive)
	{
		if (jobQueue.GetCount() > 0 && (**jobQueue.GetFront()).d == 0)
		{
			jobQueue.Pop()->f();
		}
	}


	runningThread = nullptr;
}


ScratchEngine::JobSystem::JobSystem() : JobSystem(__max(0, thread::hardware_concurrency() - 1))
{
}

ScratchEngine::JobSystem::JobSystem(u32 maxNumWorkerThreads) : jobAllocator(maxNumWorkerThreads * 16), workerThreads(reinterpret_cast<WorkerThread*>(malloc(maxNumWorkerThreads * sizeof(WorkerThread)))), maxNumWorkerThreads(maxNumWorkerThreads), numActivatedWorkerThreads(0)
{
	for (u32 i = 0; i < maxNumWorkerThreads; ++i)
		new (&workerThreads[i]) WorkerThread(2 * 16);
}

ScratchEngine::JobSystem::~JobSystem()
{
	free(workerThreads);
}
