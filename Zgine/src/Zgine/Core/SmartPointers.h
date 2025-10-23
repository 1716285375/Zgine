#pragma once

#include <memory>
#include <vector>
#include <array>

namespace Zgine {

	/**
	 * @brief Smart pointer aliases for better code readability and consistency
	 * @details This header provides type aliases and helper functions for smart pointers
	 *          to improve code readability and maintain consistency across the engine
	 */

	/**
	 * @brief Reference-counted smart pointer alias
	 * @tparam T The type of object to manage
	 */
	template<typename T>
	using Ref = std::shared_ptr<T>;

	/**
	 * @brief Unique ownership smart pointer alias
	 * @tparam T The type of object to manage
	 */
	template<typename T>
	using Scope = std::unique_ptr<T>;

	/**
	 * @brief Array specialization for unique ownership
	 * @tparam T The type of array elements
	 */
	template<typename T>
	using ScopeArray = std::unique_ptr<T[]>;

	/**
	 * @brief Weak reference smart pointer alias
	 * @tparam T The type of object to reference weakly
	 */
	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	/**
	 * @brief Helper function for creating reference-counted smart pointers
	 * @tparam T The type of object to create
	 * @tparam Args The types of constructor arguments
	 * @param args The constructor arguments
	 * @return Ref<T> A reference-counted smart pointer to the created object
	 */
	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	/**
	 * @brief Helper function for creating unique ownership smart pointers
	 * @tparam T The type of object to create
	 * @tparam Args The types of constructor arguments
	 * @param args The constructor arguments
	 * @return Scope<T> A unique ownership smart pointer to the created object
	 */
	template<typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	/**
	 * @brief Helper function for creating unique ownership arrays
	 * @tparam T The type of array elements
	 * @param size The size of the array to create
	 * @return ScopeArray<T> A unique ownership smart pointer to the created array
	 */
	template<typename T>
	constexpr ScopeArray<T> CreateScopeArray(size_t size)
	{
		return std::make_unique<T[]>(size);
	}

	/**
	 * @brief Helper function for creating weak references
	 * @tparam T The type of object to reference weakly
	 * @param ref The reference-counted smart pointer to create a weak reference from
	 * @return WeakRef<T> A weak reference to the object
	 */
	template<typename T>
	constexpr WeakRef<T> CreateWeakRef(const Ref<T>& ref)
	{
		return WeakRef<T>(ref);
	}

	/**
	 * @brief Vector of reference-counted smart pointers
	 * @tparam T The type of objects to manage
	 */
	template<typename T>
	using RefVector = std::vector<Ref<T>>;

	/**
	 * @brief Vector of unique ownership smart pointers
	 * @tparam T The type of objects to manage
	 */
	template<typename T>
	using ScopeVector = std::vector<Scope<T>>;

	/**
	 * @brief Array of reference-counted smart pointers
	 * @tparam T The type of objects to manage
	 * @tparam N The size of the array
	 */
	template<typename T, size_t N>
	using RefArray = std::array<Ref<T>, N>;

	/**
	 * @brief Array of unique ownership smart pointers
	 * @tparam T The type of objects to manage
	 * @tparam N The size of the array
	 */
	template<typename T, size_t N>
	using ScopeArray = std::array<Scope<T>, N>;

	/**
	 * @brief Helper function for creating a vector of reference-counted smart pointers
	 * @tparam T The type of objects to create
	 * @tparam Args The types of constructor arguments
	 * @param count The number of objects to create
	 * @param args The constructor arguments for each object
	 * @return RefVector<T> A vector of reference-counted smart pointers
	 */
	template<typename T, typename... Args>
	RefVector<T> CreateRefVector(size_t count, Args&&... args)
	{
		RefVector<T> result;
		result.reserve(count);
		for (size_t i = 0; i < count; ++i)
		{
			result.emplace_back(CreateRef<T>(std::forward<Args>(args)...));
		}
		return result;
	}

	/**
	 * @brief Helper function for creating a vector of unique ownership smart pointers
	 * @tparam T The type of objects to create
	 * @tparam Args The types of constructor arguments
	 * @param count The number of objects to create
	 * @param args The constructor arguments for each object
	 * @return ScopeVector<T> A vector of unique ownership smart pointers
	 */
	template<typename T, typename... Args>
	ScopeVector<T> CreateScopeVector(size_t count, Args&&... args)
	{
		ScopeVector<T> result;
		result.reserve(count);
		for (size_t i = 0; i < count; ++i)
		{
			result.emplace_back(CreateScope<T>(std::forward<Args>(args)...));
		}
		return result;
	}

}
