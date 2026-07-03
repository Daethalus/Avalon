#pragma once

#include <type_traits>
#include "Avalon/Common.hpp"

namespace Avalon
{
	class IAllocator
	{
	public:
		virtual ~IAllocator() = default;

		virtual VoidPtr MemAlloc(usize bytes) = 0;
		virtual void    MemFree(VoidPtr ptr) = 0;
		virtual VoidPtr MemRealloc(VoidPtr ptr, usize newSize) = 0;
	};


	namespace MemoryGlobals
	{
		AV_API IAllocator* GetGeneralPurposeAllocator();
	}

	template <typename Type>
	void DestroyAndFree(Type* type)
	{
		if constexpr (std::is_destructible_v<Type>)
		{
			type->~Type();
		}
		IAllocator* alloc = MemoryGlobals::GetGeneralPurposeAllocator();
		alloc->MemFree(type);
	}

	template <typename Type, typename... Args>
	Type* Alloc(Args&&... args)
	{
		IAllocator* alloc = MemoryGlobals::GetGeneralPurposeAllocator();
		auto        ptr = static_cast<Type*>(alloc->MemAlloc(sizeof(Type)));

		if constexpr (std::is_aggregate_v<Type>)
		{
			new(ptr) Type{std::forward<Args>(args)...};
		}
		else
		{
			new(ptr) Type(std::forward<Args>(args)...);
		}
		return ptr;
	}

	template <typename Type>
	void DestroyAndFree(const Type* type)
	{
		if constexpr (std::is_destructible_v<Type>)
		{
			type->~Type();
		}
		IAllocator* alloc = MemoryGlobals::GetGeneralPurposeAllocator();
		alloc->MemFree(const_cast<Type*>(type));
	}
}
