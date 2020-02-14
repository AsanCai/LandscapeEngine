#pragma once

#include <string>

namespace LandscapeEngine{
	class BaseEvent {
	public:
		virtual const std::string & getName() const = 0;

	protected:
		// identifier of the event
		std::string _name;
	};
}