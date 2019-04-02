#include "Barrier.h"

ScratchEngine::Multithreading::Barrier::Barrier(size_t n) : threshold(n), count(n), generation(0)
{
}

void ScratchEngine::Multithreading::Barrier::Wait()
{
	unique_lock<std::mutex> lock{m};
	size_t previousGeneration = generation;

	if (!--count)
	{
		generation++;
		count = threshold;
		cv.notify_all();
	}
	else
		cv.wait(lock, [this, previousGeneration] { return previousGeneration != generation; });
}
