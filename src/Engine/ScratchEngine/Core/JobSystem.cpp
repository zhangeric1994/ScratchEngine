#include "JobSystem.h"


ScratchEngine::Job::Job(function<void()> f) : f(f), status(JobStatus::Done)
{
}


ScratchEngine::WorkerThread::WorkerThread(u32 capacity) : jobQueue(capacity), runningThread(nullptr), isActive(false)
{
}

void ScratchEngine::WorkerThread::Work()
{
	while (isActive)
	{
		if (jobQueue.GetCount() > 0)
		{
			Job* job = jobQueue.Pop();

			job->f();
			job->status = JobStatus::Done;
			job->cv.notify_all();
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
		new (&workerThreads[i]) WorkerThread(2 * 16 - 1);
}

ScratchEngine::JobSystem::~JobSystem()
{
	free(workerThreads);
}
