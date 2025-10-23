#pragma once

#include "Zgine/Core.h"
#include <spdlog/fmt/fmt.h>
#include <functional>
#include <string>
#include <iostream>

namespace Zgine {

	/**
	 * @brief Enumeration of all event types in the engine
	 * @details Defines all possible event types that can occur in the application
	 */
	enum class EventType
	{
		None = 0,                    ///< No event
		WindowClose,                 ///< Window close event
		WindowResize,                ///< Window resize event
		WindowFocus,                 ///< Window focus event
		WindowLostFocus,             ///< Window lost focus event
		WindowMoved,                 ///< Window moved event
		AppTick,                     ///< Application tick event
		AppUpdate,                   ///< Application update event
		AppRender,                   ///< Application render event
		KeyPressed,                  ///< Key pressed event
		KeyReleased,                 ///< Key released event
		KeyTyped,                    ///< Key typed event
		MouseButtonPressed,          ///< Mouse button pressed event
		MouseButtonReleased,         ///< Mouse button released event
		MouseMoved,                  ///< Mouse moved event
		MouseScrolled                ///< Mouse scrolled event
	};

	/**
	 * @brief Event category flags for event classification
	 * @details Used to categorize events for efficient filtering and handling
	 */
	enum EventCategory
	{
		None = 0,                           ///< No category
		EventCategoryApplication = BIT(0),  ///< Application events
		EventCategoryInput = BIT(1),        ///< Input events
		EventCategoryKeyboard = BIT(2),     ///< Keyboard events
		EventCategoryMouse = BIT(3),        ///< Mouse events
		EventCategoryMouseButton = BIT(4)    ///< Mouse button events
	};

	/**
	 * @brief Macro for defining event type information
	 * @param type The event type name
	 * @details Generates static type getter, virtual type getter, and name getter
	 */
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type;}\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

	/**
	 * @brief Macro for defining event category information
	 * @param category The event category flag
	 * @details Generates virtual category flags getter
	 */
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category;}

	/**
	 * @brief Base event class for the engine's event system
	 * @details This abstract base class defines the interface for all events
	 *          in the engine's event handling system
	 */
	class ZG_API Event
	{
		friend class EventDispatcher;
	public:
		/**
		 * @brief Get the type of this event
		 * @return EventType The event type
		 */
		virtual EventType GetEventType() const = 0;
		
		/**
		 * @brief Get the name of this event
		 * @return const char* The event name
		 */
		virtual const char* GetName() const = 0;
		
		/**
		 * @brief Get the category flags for this event
		 * @return int The category flags
		 */
		virtual int GetCategoryFlags() const = 0;
		
		/**
		 * @brief Convert the event to a string representation
		 * @return std::string String representation of the event
		 */
		virtual std::string ToString() const { return GetName(); }

		/**
		 * @brief Check if this event belongs to a specific category
		 * @param category The category to check
		 * @return bool True if the event belongs to the category
		 */
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		/**
		 * @brief Check if this event has been handled
		 * @return bool True if the event has been handled
		 */
		bool Handled()const { return m_Handled; }
		
	protected:
		bool m_Handled = false;  ///< Whether this event has been handled
	};

	/**
	 * @brief Event dispatcher for routing events to appropriate handlers
	 * @details This class provides a type-safe way to dispatch events to
	 *          specific event handlers based on event type
	 */
	class EventDispatcher
	{
		/**
		 * @brief Event handler function type
		 * @tparam T The event type
		 */
		template<typename T>
		using EventFn = std::function<bool(T&)>;
		
	public:
		/**
		 * @brief Construct a new EventDispatcher object
		 * @param event The event to dispatch
		 */
		EventDispatcher(Event& event)
			: m_Event(event)
		{
			// EventDispatcher constructor implementation
		}

		/**
		 * @brief Dispatch the event to a handler function
		 * @tparam T The event type to dispatch
		 * @param func The handler function
		 * @return bool True if the event was dispatched and handled
		 * @details Only dispatches if the event type matches the handler type
		 */
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*static_cast<T*>(& m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;  ///< Reference to the event being dispatched
	};

	/**
	 * @brief Stream output operator for events
	 * @param os The output stream
	 * @param e The event to output
	 * @return std::ostream& Reference to the output stream
	 */
	inline std::ostream& operator<< (std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

}

// fmt::formatter Specialization
/**
 * @brief fmt::formatter specialization for Zgine::Event
 * @details Enables formatting of Event objects using fmt library
 */
template <>
struct fmt::formatter<Zgine::Event> {
	/**
	 * @brief Parse format string
	 * @param ctx The format parse context
	 * @return constexpr auto Iterator to end of parsed format
	 */
	constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

	/**
	 * @brief Format the event
	 * @tparam FormatContext The format context type
	 * @param e The event to format
	 * @param ctx The format context
	 * @return auto Iterator to end of formatted output
	 */
	template <typename FormatContext>
	auto format(const Zgine::Event& e, FormatContext& ctx) const ->
		decltype(ctx.out()) {
		// use Event function ToString 
		return fmt::format_to(ctx.out(), "{}", e.ToString());
	}
};