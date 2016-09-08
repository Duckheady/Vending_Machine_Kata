/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Handles cross-module communication in the system. Just a simple event system.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
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

