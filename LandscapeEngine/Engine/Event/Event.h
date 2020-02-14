#pragma once

#include <Event/BaseEvent.h>

#include <functional>

namespace LandscapeEngine {
	template <typename ...args>
	class Event :public BaseEvent {
	public:
		using Callback = std::function<void(args...)>;

		explicit Event(const std::string & name, const Callback & cb){
			_name = name;
			_callbackFunction = cb;
		}

		virtual const std::string &getName() const {
			return _name;
		}

		void trigger(args... arguments) {
			_callbackFunction(arguments...);
		}
	private:
		// callback function of the event
		Callback _callbackFunction;
	};
}