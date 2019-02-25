#pragma once

namespace ScratchEngine
{
	namespace Memory
	{
		typedef char i8;
		typedef i8 byte;
		typedef short i16;
		typedef int i32;
		typedef long long i64;

		typedef unsigned char u8;
		typedef unsigned short u16;
		typedef unsigned int u32;
		typedef unsigned long long u64;

#if defined(_WIN64)
		typedef u64 uptr;
#else
		typedef W64SAFE u32 uptr;
#endif

		typedef float f32;
		typedef double f64;

		typedef u32 Bool;     // Defines: True and False
		typedef void* Handle;
		typedef u32 Error;

		typedef u32 Id;      // Local id, GUIDs are too slow and unnecessary for our purposes.

		typedef char* pstr;
		typedef const char* pcstr;

		typedef void(*Callback)(void* pUserData);
	}
}