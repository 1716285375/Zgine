#pragma once

#include <memory>

namespace Zgine {

	// Smart pointer aliases for better code readability and consistency
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;

	// Helper functions for creating smart pointers
	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	// Array specialization for Scope
	template<typename T>
	using ScopeArray = std::unique_ptr<T[]>;

	template<typename T>
	constexpr ScopeArray<T> CreateScopeArray(size_t size)
	{
		return std::make_unique<T[]>(size);
	}

}
