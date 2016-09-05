#include "EventSystem.hpp"

DefineSingleton(EventSystem);

EventSystem::~EventSystem()
{
  /*foreach callbackPackage (regardless of event type) delete*/
  for(auto eventIter = callbacks.begin(); eventIter != callbacks.end(); ++eventIter)
  {
    for(auto callbackIter = eventIter->second.begin(); callbackIter != eventIter->second.end(); ++callbackIter)
      delete *callbackIter;
    eventIter->second.clear();
  }
  callbacks.clear();
}

void EventSystem::RegisterCallbackPackage(TYPEID id, CallbackPackage* package)
{
  callbacks[id].push_back(package);
}

void EventSystem::BroadcastEvent(const Event* event)
{
  TYPEID typeId = event->eventId;
  /*Find the event's listener list*/
  auto eventIter = callbacks.find(typeId);
  if(eventIter != callbacks.end())
  {
    /*foreach CallbackPackage in this event's callback list*/
    for(auto callbackIter = eventIter->second.begin(); callbackIter != eventIter->second.end(); ++callbackIter)
      (*callbackIter)->InvokeCallback(event);
  }
}

