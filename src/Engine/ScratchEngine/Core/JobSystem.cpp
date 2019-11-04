#include "JobSystem.h"


ScratchEngine::WorkerThread::WorkerThread(u32 capacity) : jobQueue(capacity), runningThread(nullptr), isActive(false)
{
}

void ScratchEngine::WorkerThread::Do()
{
	while (isActive)
	{
		if (jobQueue.GetCount())
		{
			Job* job = jobQueue.Pop();

			job->f();
		}
	}

	runningThread = nullptr;
}


ScratchEngine::JobSystem::JobSystem() : JobSystem(__max(0, thread::hardware_concurrency() - 1))
{
}

ScratchEngine::JobSystem::JobSystem(u32 maxNumWorkerThreads) : jobAllocator(maxNumWorkerThreads * 16), workerThreads(reinterpret_cast<WorkerThread*>(malloc(maxNumWorkerThreads * sizeof(WorkerThread))))
{
	this->maxNumWorkerThreads = maxNumWorkerThreads;
	this->numActivatedWorkerThreads = 0;
	this->nextJobID = 0;


	for (u32 i = 0; i < maxNumWorkerThreads; ++i)
		new (&workerThreads[i]) WorkerThread(16);
}
