#pragma once

#include "Zgine/Core.h"
#include <spdlog/fmt/fmt.h>

namespace Zgine {


	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = ZG_BIT(0),
		EventCategoryInput = ZG_BIT(1),
		EventCategoryKeyboard = ZG_BIT(2),
		EventCategoryMouse = ZG_BIT(3),
		EventCategoryMouseButton = ZG_BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type;}\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category;}

	class ZG_API Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		bool Handled()const { return m_Handled; }
	protected:
		bool m_Handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{

		}

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
		Event& m_Event;
	};

	inline std::ostream& operator<< (std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}


}

// fmt::formatter Specialization
template <>
struct fmt::formatter<Zgine::Event> {
	constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(const Zgine::Event& e, FormatContext& ctx) const ->
		decltype(ctx.out()) {
		// use Event function ToString 
		return fmt::format_to(ctx.out(), "{}", e.ToString());
	}
};