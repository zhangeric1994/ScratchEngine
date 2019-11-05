#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H
#pragma once

#include "../header.h"

#include "../Common/LockFreeQueue.hpp"
#include "../Common/Typedefs.h"
#include "../Memory/PoolAllocator.hpp"

using namespace ScratchEngine::Multithreading;


namespace ScratchEngine
{
	enum class JobStatus : u32
	{
		Done = 0,
		Scheduled = 1,
		InProgress = 2,
	};


	struct Job
	{
		friend class JobSystem;
		friend class WorkerThread;


	private:
		function<void()> const f;
		JobStatus status;
		mutex m;
		condition_variable cv;


	public:
		Job(function<void()> f);

		void Wait();
	};


	class WorkerThread
	{
	private:
		LockFreeQueue<Job*> jobQueue;
		thread* runningThread;
		atomic<bool> isActive;


	public:
		WorkerThread(u32 capacity);


		bool IsRunning() const;
		bool IsActive() const;

		bool Schedule(Job* const job);

		void Activate();
		void Deactivate();

	private:
		void Work();
	};


	class JobSystem
	{
	private:
		PoolAllocator<sizeof(Job)> jobAllocator;
		WorkerThread* workerThreads;
		u32 maxNumWorkerThreads;
		u32 numActivatedWorkerThreads;

		void __Schedule(Job* job, u32 threadID);
		Job* __Execute(function<void()> const& f, u32 threadID);


	public:
		JobSystem();
		JobSystem(u32 maxNumWorkerThreads);
		~JobSystem();


		u32 GetMaxNumWorkerThreads() const;
		u32 GetNumActivatedWorkerThreads() const;

		void ActivateThreads(u32 numWorkerThreads);
		void ActivateAllThreads();
		void DeactivateThreads(u32 numWorkerThreads);
		void DeactivateAllThreads();

		void Schedule(Job* job);
		void Schedule(Job* job, u32 threadID);
		void Schedule(Job* job, u32 threadLowerBound, u32 threadUpperBound);

		Job* Execute(function<void()> const& f);
		Job* Execute(function<void()> const& f, u32 threadID);
		Job* Execute(function<void()> const& f, u32 threadLowerBound, u32 threadUpperBound);
	};
}


inline void ScratchEngine::Job::Wait()
{
	if (status == JobStatus::Done)
		return;
	

	unique_lock<std::mutex> lock { m };
	cv.wait(lock);
}

inline bool ScratchEngine::WorkerThread::IsRunning() const
{
	return runningThread != nullptr;
}

inline bool ScratchEngine::WorkerThread::IsActive() const
{
	return isActive;
}

inline bool ScratchEngine::WorkerThread::Schedule(Job* const job)
{
	bool result = jobQueue.Push(job);

	if (result)
		job->status = JobStatus::Scheduled;

	return result;
}

inline void ScratchEngine::WorkerThread::Activate()
{
	isActive = true;

	if (!runningThread)
		runningThread = new thread(&WorkerThread::Work, static_cast<WorkerThread*>(this));
}

inline void ScratchEngine::WorkerThread::Deactivate()
{
	isActive = false;
}


inline void ScratchEngine::JobSystem::__Schedule(Job* job, u32 threadID)
{
	workerThreads[threadID].Schedule(job);
}

inline Job* ScratchEngine::JobSystem::__Execute(function<void()> const& f, u32 threadID)
{
	if (numActivatedWorkerThreads == 0)
	{
		f();

		return nullptr;
	}


	Job* job = reinterpret_cast<Job*>(jobAllocator.Allocate());

	new (job) Job(f);


	__Schedule(job, threadID);


	return job;
}

inline u32 ScratchEngine::JobSystem::GetMaxNumWorkerThreads() const
{
	return maxNumWorkerThreads;
}

inline u32 ScratchEngine::JobSystem::GetNumActivatedWorkerThreads() const
{
	return numActivatedWorkerThreads;
}

inline void ScratchEngine::JobSystem::ActivateThreads(u32 numWorkerThreads)
{
	u32 N = maxNumWorkerThreads - numActivatedWorkerThreads;

	if (N > 0)
	{
		N = __min(N, numWorkerThreads);

		for (u32 i = 0; i < N; ++i)
			workerThreads[numActivatedWorkerThreads + i].Activate();


		numActivatedWorkerThreads += N;
	}
}

inline void ScratchEngine::JobSystem::ActivateAllThreads()
{
	u32 N = maxNumWorkerThreads - numActivatedWorkerThreads;

	if (N > 0)
	{
		for (u32 i = 0; i < N; ++i)
			workerThreads[numActivatedWorkerThreads + i].Activate();


		numActivatedWorkerThreads = maxNumWorkerThreads;
	}
}

inline void ScratchEngine::JobSystem::DeactivateThreads(u32 numWorkerThreads)
{
	u32 N = __min(numActivatedWorkerThreads, numWorkerThreads);

	for (u32 i = 0; i < N; ++i)
		workerThreads[numActivatedWorkerThreads - 1 - i].Deactivate();


	numActivatedWorkerThreads -= N;
}

inline void ScratchEngine::JobSystem::DeactivateAllThreads()
{
	for (u32 i = 0; i < numActivatedWorkerThreads; ++i)
		workerThreads[i].Deactivate();


	numActivatedWorkerThreads = 0;
}

inline void ScratchEngine::JobSystem::Schedule(Job* job)
{
	_ASSERT(job->status == JobStatus::Done);


	srand(time(0));

	__Schedule(job, rand() % numActivatedWorkerThreads);
}

inline void ScratchEngine::JobSystem::Schedule(Job* job, u32 threadID)
{
	_ASSERT(job->status == JobStatus::Done);
	_ASSERT(threadID < numActivatedWorkerThreads);


	__Schedule(job, rand() % numActivatedWorkerThreads);
}

inline void ScratchEngine::JobSystem::Schedule(Job* job, u32 threadLowerBound, u32 threadUpperBound)
{
	_ASSERT(job->status == JobStatus::Done);
	_ASSERT(threadLowerBound <= threadUpperBound);
	_ASSERT(threadLowerBound < numActivatedWorkerThreads);


	if (threadLowerBound == threadUpperBound)
		__Schedule(job, threadLowerBound);
	else
	{
		int N = __min(numActivatedWorkerThreads - 1, threadUpperBound) - threadLowerBound;


		srand(time(0));

		__Schedule(job, threadLowerBound + rand() % (N + 1));
	}
}

inline Job* ScratchEngine::JobSystem::Execute(function<void()> const& f)
{
	srand(time(0));

	return __Execute(f, rand() % numActivatedWorkerThreads);
}

inline Job* ScratchEngine::JobSystem::Execute(function<void()> const& f, u32 threadID)
{
	_ASSERT(threadID < numActivatedWorkerThreads);


	return __Execute(f, threadID);
}

inline Job* ScratchEngine::JobSystem::Execute(function<void()> const& f, u32 threadLowerBound, u32 threadUpperBound)
{
	_ASSERT(threadLowerBound <= threadUpperBound);
	_ASSERT(threadLowerBound < numActivatedWorkerThreads);


	if (threadLowerBound == threadUpperBound)
		return __Execute(f, threadLowerBound);


	int N = __min(numActivatedWorkerThreads - 1, threadUpperBound) - threadLowerBound;


	srand(time(0));

	return __Execute(f, threadLowerBound + rand() % (N + 1));
}

#endif // !JOB_SYSTEM_H