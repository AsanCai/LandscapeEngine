#pragma once

#include <Event/Event.h>

#include <string>
#include <map>
#include <vector>

namespace LandscapeEngine{
	class Dispatcher {
	public:
		~Dispatcher() {
			for (auto el : _eventList) {
				for (auto e : el.second)
					delete e;
			}
		}

		static Dispatcher* getInstance() {
			if(_instance == nullptr) {
				_instance = new Dispatcher();
			}

			return _instance;
		}

		void registerEvent(BaseEvent *event) {
			if (event)
				_eventList[event->getName()].push_back(event);
		}

		template <typename ...args>
		void dispatchEvent(const std::string & eventName, args...arguments) {
			auto it_eventList = _eventList.find(eventName);
			if (it_eventList == _eventList.end())
				return;
			for (auto ie : it_eventList->second) {
				if (Event<args...> *event = dynamic_cast<Event<args...>*>(ie))
					event->trigger(arguments...);
			}
		}

	private:
		// single instance
		static Dispatcher *_instance;

		// store all events
		std::map<std::string, std::vector<BaseEvent*>> _eventList;
	
	private:
		// make constructor private
		Dispatcher() {}
	};

	Dispatcher *Dispatcher::_instance = nullptr;
}