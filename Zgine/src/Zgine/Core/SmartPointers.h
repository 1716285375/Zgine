#pragma once

#include <memory>
#include <vector>
#include <array>
#include <functional>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <exception>
#include <new>
#include <cstdlib>

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
	 * @brief Fixed-size array of unique ownership smart pointers
	 * @tparam T The type of objects to manage
	 * @tparam N The size of the array
	 */
	template<typename T, size_t N>
	using ScopeFixedArray = std::array<Scope<T>, N>;

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

	// ============================================================================
	// Custom Deleter Support
	// ============================================================================

	/**
	 * @brief Helper function for creating reference-counted smart pointers with custom deleter
	 * @tparam T The type of object to create
	 * @tparam Deleter The type of custom deleter
	 * @tparam Args The types of constructor arguments
	 * @param deleter Custom deleter function
	 * @param args The constructor arguments
	 * @return Ref<T> A reference-counted smart pointer with custom deleter
	 */
	template<typename T, typename Deleter, typename... Args>
	constexpr Ref<T> CreateRefWithDeleter(Deleter&& deleter, Args&&... args)
	{
		return std::shared_ptr<T>(new T(std::forward<Args>(args)...), std::forward<Deleter>(deleter));
	}

	/**
	 * @brief Helper function for creating unique ownership smart pointers with custom deleter
	 * @tparam T The type of object to create
	 * @tparam Deleter The type of custom deleter
	 * @tparam Args The types of constructor arguments
	 * @param deleter Custom deleter function
	 * @param args The constructor arguments
	 * @return Scope<T> A unique ownership smart pointer with custom deleter
	 */
	template<typename T, typename Deleter, typename... Args>
	constexpr Scope<T> CreateScopeWithDeleter(Deleter&& deleter, Args&&... args)
	{
		return std::unique_ptr<T, std::decay_t<Deleter>>(new T(std::forward<Args>(args)...), std::forward<Deleter>(deleter));
	}

	// ============================================================================
	// Polymorphic Support and Type Conversion
	// ============================================================================

	/**
	 * @brief Safe dynamic cast for reference-counted smart pointers
	 * @tparam To The target type
	 * @tparam From The source type
	 * @param ptr The source smart pointer
	 * @return Ref<To> Casted smart pointer, or nullptr if cast fails
	 */
	template<typename To, typename From>
	Ref<To> DynamicRefCast(const Ref<From>& ptr)
	{
		return std::dynamic_pointer_cast<To>(ptr);
	}

	/**
	 * @brief Safe static cast for reference-counted smart pointers
	 * @tparam To The target type
	 * @tparam From The source type
	 * @param ptr The source smart pointer
	 * @return Ref<To> Casted smart pointer
	 */
	template<typename To, typename From>
	Ref<To> StaticRefCast(const Ref<From>& ptr)
	{
		return std::static_pointer_cast<To>(ptr);
	}

	/**
	 * @brief Safe const cast for reference-counted smart pointers
	 * @tparam To The target type
	 * @tparam From The source type
	 * @param ptr The source smart pointer
	 * @return Ref<To> Casted smart pointer
	 */
	template<typename To, typename From>
	Ref<To> ConstRefCast(const Ref<From>& ptr)
	{
		return std::const_pointer_cast<To>(ptr);
	}

	/**
	 * @brief Safe reinterpret cast for reference-counted smart pointers
	 * @tparam To The target type
	 * @tparam From The source type
	 * @param ptr The source smart pointer
	 * @return Ref<To> Casted smart pointer
	 */
	template<typename To, typename From>
	Ref<To> ReinterpretRefCast(const Ref<From>& ptr)
	{
		return std::reinterpret_pointer_cast<To>(ptr);
	}

	// ============================================================================
	// Utility Functions and Operators
	// ============================================================================

	/**
	 * @brief Check if a smart pointer is valid (not null)
	 * @tparam T The type of object
	 * @param ptr The smart pointer to check
	 * @return true if pointer is valid, false otherwise
	 */
	template<typename T>
	constexpr bool IsValid(const Ref<T>& ptr) noexcept
	{
		return ptr != nullptr;
	}

	/**
	 * @brief Check if a smart pointer is valid (not null)
	 * @tparam T The type of object
	 * @param ptr The smart pointer to check
	 * @return true if pointer is valid, false otherwise
	 */
	template<typename T>
	constexpr bool IsValid(const Scope<T>& ptr) noexcept
	{
		return ptr != nullptr;
	}

	/**
	 * @brief Check if a smart pointer is valid (not null)
	 * @tparam T The type of object
	 * @param ptr The smart pointer to check
	 * @return true if pointer is valid, false otherwise
	 */
	template<typename T>
	constexpr bool IsValid(const WeakRef<T>& ptr) noexcept
	{
		return !ptr.expired();
	}

	/**
	 * @brief Get the raw pointer from a smart pointer safely
	 * @tparam T The type of object
	 * @param ptr The smart pointer
	 * @return Raw pointer or nullptr if smart pointer is null
	 */
	template<typename T>
	constexpr T* GetRawPtr(const Ref<T>& ptr) noexcept
	{
		return ptr.get();
	}

	/**
	 * @brief Get the raw pointer from a smart pointer safely
	 * @tparam T The type of object
	 * @param ptr The smart pointer
	 * @return Raw pointer or nullptr if smart pointer is null
	 */
	template<typename T>
	constexpr T* GetRawPtr(const Scope<T>& ptr) noexcept
	{
		return ptr.get();
	}

	/**
	 * @brief Get the raw pointer from a weak reference safely
	 * @tparam T The type of object
	 * @param ptr The weak reference
	 * @return Raw pointer or nullptr if weak reference is expired
	 */
	template<typename T>
	constexpr T* GetRawPtr(const WeakRef<T>& ptr) noexcept
	{
		auto shared = ptr.lock();
		return shared ? shared.get() : nullptr;
	}

	/**
	 * @brief Reset a smart pointer to null
	 * @tparam T The type of object
	 * @param ptr The smart pointer to reset
	 */
	template<typename T>
	constexpr void Reset(Ref<T>& ptr) noexcept
	{
		ptr.reset();
	}

	/**
	 * @brief Reset a smart pointer to null
	 * @tparam T The type of object
	 * @param ptr The smart pointer to reset
	 */
	template<typename T>
	constexpr void Reset(Scope<T>& ptr) noexcept
	{
		ptr.reset();
	}

	/**
	 * @brief Swap two smart pointers
	 * @tparam T The type of object
	 * @param lhs First smart pointer
	 * @param rhs Second smart pointer
	 */
	template<typename T>
	constexpr void Swap(Ref<T>& lhs, Ref<T>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	/**
	 * @brief Swap two smart pointers
	 * @tparam T The type of object
	 * @param lhs First smart pointer
	 * @param rhs Second smart pointer
	 */
	template<typename T>
	constexpr void Swap(Scope<T>& lhs, Scope<T>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	// ============================================================================
	// Performance and Memory Management Features
	// ============================================================================

	/**
	 * @brief Create a smart pointer with aligned memory allocation
	 * @tparam T The type of object to create
	 * @tparam Args The types of constructor arguments
	 * @param alignment Memory alignment requirement
	 * @param args The constructor arguments
	 * @return Ref<T> A reference-counted smart pointer with aligned memory
	 */
	template<typename T, typename... Args>
	Ref<T> CreateAlignedRef(size_t alignment, Args&&... args)
	{
		static_assert(std::is_trivially_destructible_v<T>, "T must be trivially destructible for aligned allocation");
		
		void* ptr = std::aligned_alloc(alignment, sizeof(T));
		if (!ptr) {
			throw std::bad_alloc();
		}
		
		new(ptr) T(std::forward<Args>(args)...);
		
		return std::shared_ptr<T>(static_cast<T*>(ptr), [](T* p) {
			p->~T();
			std::free(p);
		});
	}

	/**
	 * @brief Create a smart pointer with custom allocator
	 * @tparam T The type of object to create
	 * @tparam Allocator The type of allocator
	 * @tparam Args The types of constructor arguments
	 * @param allocator Custom allocator
	 * @param args The constructor arguments
	 * @return Ref<T> A reference-counted smart pointer with custom allocator
	 */
	template<typename T, typename Allocator, typename... Args>
	Ref<T> CreateRefWithAllocator(Allocator& allocator, Args&&... args)
	{
		using AllocatorTraits = std::allocator_traits<Allocator>;
		using RebindAllocator = typename AllocatorTraits::template rebind_alloc<T>;
		
		RebindAllocator rebindAlloc(allocator);
		T* ptr = AllocatorTraits::allocate(rebindAlloc, 1);
		
		try {
			AllocatorTraits::construct(rebindAlloc, ptr, std::forward<Args>(args)...);
		} catch (...) {
			AllocatorTraits::deallocate(rebindAlloc, ptr, 1);
			throw;
		}
		
		return std::shared_ptr<T>(ptr, [rebindAlloc](T* p) mutable {
			AllocatorTraits::destroy(rebindAlloc, p);
			AllocatorTraits::deallocate(rebindAlloc, p, 1);
		});
	}

	// ============================================================================
	// Debug and Diagnostic Features
	// ============================================================================

	/**
	 * @brief Get reference count information for debugging
	 * @tparam T The type of object
	 * @param ptr The smart pointer
	 * @return Reference count information
	 */
	template<typename T>
	struct RefCountInfo
	{
		long use_count;
		long weak_count;
		bool expired;
		
		RefCountInfo(long use, long weak, bool exp) 
			: use_count(use), weak_count(weak), expired(exp) {}
	};

	template<typename T>
	RefCountInfo<T> GetRefCountInfo(const Ref<T>& ptr) noexcept
	{
		return RefCountInfo<T>(ptr.use_count(), 0, false);
	}

	template<typename T>
	RefCountInfo<T> GetRefCountInfo(const WeakRef<T>& ptr) noexcept
	{
		auto shared = ptr.lock();
		return RefCountInfo<T>(
			shared ? shared.use_count() : 0,
			ptr.use_count(),
			ptr.expired()
		);
	}

	/**
	 * @brief Debug helper to check for circular references
	 * @tparam T The type of object
	 * @param ptr The smart pointer to check
	 * @return true if potential circular reference detected
	 */
	template<typename T>
	bool HasCircularReference(const Ref<T>& ptr) noexcept
	{
		// Simple heuristic: if use_count is very high, might indicate circular reference
		return ptr.use_count() > 1000;
	}

	// ============================================================================
	// Exception Safety and RAII Helpers
	// ============================================================================

	/**
	 * @brief RAII helper for temporary smart pointer management
	 * @tparam T The type of object
	 */
	template<typename T>
	class SmartPtrGuard
	{
	public:
		explicit SmartPtrGuard(Ref<T>& ptr) : m_Ptr(ptr), m_Original(ptr) {}
		explicit SmartPtrGuard(Scope<T>& ptr) : m_ScopePtr(ptr), m_OriginalScope(ptr) {}
		
		~SmartPtrGuard()
		{
			if (m_Ptr) {
				m_Ptr = m_Original;
			}
			if (m_ScopePtr) {
				m_ScopePtr = std::move(m_OriginalScope);
			}
		}
		
		// Non-copyable
		SmartPtrGuard(const SmartPtrGuard&) = delete;
		SmartPtrGuard& operator=(const SmartPtrGuard&) = delete;
		
		// Non-movable
		SmartPtrGuard(SmartPtrGuard&&) = delete;
		SmartPtrGuard& operator=(SmartPtrGuard&&) = delete;

	private:
		Ref<T>* m_Ptr = nullptr;
		Ref<T> m_Original;
		Scope<T>* m_ScopePtr = nullptr;
		Scope<T> m_OriginalScope;
	};

	/**
	 * @brief Create a guard for temporary smart pointer management
	 * @tparam T The type of object
	 * @param ptr The smart pointer to guard
	 * @return SmartPtrGuard for RAII management
	 */
	template<typename T>
	SmartPtrGuard<T> CreateGuard(Ref<T>& ptr)
	{
		return SmartPtrGuard<T>(ptr);
	}

	template<typename T>
	SmartPtrGuard<T> CreateGuard(Scope<T>& ptr)
	{
		return SmartPtrGuard<T>(ptr);
	}

	// ============================================================================
	// Advanced Container Types
	// ============================================================================

	/**
	 * @brief Map of reference-counted smart pointers
	 * @tparam K The key type
	 * @tparam T The value type
	 */
	template<typename K, typename T>
	using RefMap = std::unordered_map<K, Ref<T>>;

	/**
	 * @brief Map of unique ownership smart pointers
	 * @tparam K The key type
	 * @tparam T The value type
	 */
	template<typename K, typename T>
	using ScopeMap = std::unordered_map<K, Scope<T>>;

	/**
	 * @brief Helper function for creating a map of reference-counted smart pointers
	 * @tparam K The key type
	 * @tparam T The value type
	 * @tparam Args The types of constructor arguments
	 * @param keys Vector of keys
	 * @param args The constructor arguments for each value
	 * @return RefMap<K, T> A map of reference-counted smart pointers
	 */
	template<typename K, typename T, typename... Args>
	RefMap<K, T> CreateRefMap(const std::vector<K>& keys, Args&&... args)
	{
		RefMap<K, T> result;
		for (const auto& key : keys)
		{
			result.emplace(key, CreateRef<T>(std::forward<Args>(args)...));
		}
		return result;
	}

	/**
	 * @brief Helper function for creating a map of unique ownership smart pointers
	 * @tparam K The key type
	 * @tparam T The value type
	 * @tparam Args The types of constructor arguments
	 * @param keys Vector of keys
	 * @param args The constructor arguments for each value
	 * @return ScopeMap<K, T> A map of unique ownership smart pointers
	 */
	template<typename K, typename T, typename... Args>
	ScopeMap<K, T> CreateScopeMap(const std::vector<K>& keys, Args&&... args)
	{
		ScopeMap<K, T> result;
		for (const auto& key : keys)
		{
			result.emplace(key, CreateScope<T>(std::forward<Args>(args)...));
		}
		return result;
	}

}
