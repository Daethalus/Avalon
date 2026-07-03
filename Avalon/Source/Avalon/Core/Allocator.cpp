#include "Allocator.hpp"

#include "mimalloc.h"


namespace Avalon
{
	class GeneralPurposeAllocator : public IAllocator
	{
	public:
		VoidPtr MemAlloc(usize bytes) override
		{
			return mi_malloc(bytes);
		}

		void MemFree(VoidPtr ptr) override
		{
			mi_free(ptr);
		}

		VoidPtr MemRealloc(VoidPtr ptr, usize newSize) override
		{
			return mi_realloc(ptr, newSize);
		}
	};

	static GeneralPurposeAllocator generalPurposeAllocator = {};

	IAllocator* MemoryGlobals::GetGeneralPurposeAllocator()
	{
		return &generalPurposeAllocator;
	}
}
