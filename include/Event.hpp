#pragma once
#include "TypeId.hpp"
class EventSystem;
class Event
{
  friend EventSystem;
  TYPEID eventId;
public:
  Event(TYPEID id) : eventId(id) {}
  virtual ~Event() {}
};
