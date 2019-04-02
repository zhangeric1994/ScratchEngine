#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H
#pragma once

namespace ScratchEngine
{
	namespace Multithreading
	{
		class WorkerThread
		{
		private:
			int id;
			bool isActive;


		public:
			void operator()();
		};
	}
}
#endif
