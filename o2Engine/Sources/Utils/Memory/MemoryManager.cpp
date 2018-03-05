#include "stdafx.h"
#include "MemoryManager.h"

#include <algorithm>

#include "Utils/Assert.h"
#include "Utils/Log/ConsoleLogStream.h"
#include "Utils/Log/FileLogStream.h"

void* operator new(size_t size, const char* location, int line)
{
	void* memory = ::operator new(size);

#if ENALBE_MEMORY_MANAGE == true
o2::MemoryManager::Instance().OnMemoryAllocate(memory, size, location, line);
#endif

	return memory;
}

void operator delete(void* allocMemory)
{
#if ENALBE_MEMORY_MANAGE == true
	o2::MemoryManager::Instance().OnMemoryRelease(allocMemory);
#endif

	free(allocMemory);
}

void operator delete(void* allocMemory, const char* location, int line)
{
	::operator delete(allocMemory);
}

void* _mmalloc(size_t size, const char* location, int line)
{
	void* memory = ::operator new(size);

#if ENALBE_MEMORY_MANAGE == true
	o2::MemoryManager::Instance().OnMemoryAllocate(memory, size, location, line);
#endif

	return memory;
}

void _mfree(void* allocMemory)
{
#if ENALBE_MEMORY_MANAGE == true
	o2::MemoryManager::Instance().OnMemoryRelease(allocMemory);
#endif

	free(allocMemory);
}

namespace o2
{
	MemoryManager::MemoryManager():
		mTotalBytes(0)
	{}

	MemoryManager::~MemoryManager()
	{
		DumpInfo();
	}

	MemoryManager& MemoryManager::Instance()
	{
		if (!mInstance)
			mInstance = new MemoryManager();

		return *mInstance;
	}

	void MemoryManager::Initialize()
	{
		mInstance = new MemoryManager();
	}

	void MemoryManager::OnMemoryAllocate(void* memory, size_t size, const char* source, int line)
	{
		AllocInfo info;
		info.memory = memory;
		info.size = size;
		info.sourceLine = line;
		info.source = source;
		mAllocs[memory] = info;
		mTotalBytes += size;
	}

	void MemoryManager::OnMemoryRelease(void* memory)
	{
		AllocsInfosMap::iterator fnd = mAllocs.find(memory);
		if (fnd != mAllocs.end())
		{
			mTotalBytes -= (*fnd).second.size;
			mAllocs.erase(fnd);
		}
	}

	void MemoryManager::DumpInfo()
	{
		printf("========MemoryManager::DumpInfo==========\n");

		printf("Total managed allocations: %f MB\n", (float)mTotalBytes / 1024.0f / 1024.0f);

		struct allocSrc
		{
			char source[128];
			int line = 0;
			int size = 0;
			int count = 0;

			bool operator<(const allocSrc& other) const
			{
				return size < other.size;
			}
		};
		std::vector<allocSrc> allocs;

		int i = 0;
		for (auto it = mAllocs.begin(); it != mAllocs.end(); ++it, i++)
		{
			auto& info = (*it).second;

			bool found = false;
			for (auto& allc : allocs)
			{
				if (allc.line == info.sourceLine && strcmp(allc.source, info.source) == 0)
				{
					allc.size += info.size;
					allc.count++;
					found = true;
					break;
				}
			}

			if (!found)
			{
				allocSrc allc;
				allc.line = info.sourceLine;
				strncpy(allc.source, info.source, 127);
				allc.size = info.size;
				allc.count = 1;

				allocs.push_back(allc);
			}
		}

		std::sort(allocs.begin(), allocs.end());

		for (int i = 0; i < (int)allocs.size(); i++)
		{
			printf("%i: %s : %i - %i bytes (%f MB) in %i allocs\n",
				   i, allocs[i].source, allocs[i].line, allocs[i].size,
				   (float)allocs[i].size / 1024.0f / 1024.0f, allocs[i].count);
		}

		printf("========END==========\n");
	}

}