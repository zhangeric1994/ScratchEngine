#ifndef BARRIER_H
#define BARRIER_H
#pragma once

#include <mutex>

using namespace std;

namespace ScratchEngine
{
	namespace Multithreading
	{
		class Barrier
		{
		private:
			mutex m;
			condition_variable cv;
			size_t threshold;
			size_t count;
			size_t generation;

		public:
			explicit Barrier(size_t n);

			void Wait();
		};
	}
}
#endif // !BARRIER_H