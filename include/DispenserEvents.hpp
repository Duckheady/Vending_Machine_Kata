#pragma once

#include "Event.hpp"

class DispenseChange : public Event
{
public:
  float changeAmount;
  DispenseChange(float change) : Event(TYPE_ID(DispenseChange)), changeAmount(change) {}
};

class DispenseItem : public Event
{
public:
  unsigned itemIndex;
  DispenseItem(unsigned index) : Event(TYPE_ID(DispenseItem)), itemIndex(index) {}
};
