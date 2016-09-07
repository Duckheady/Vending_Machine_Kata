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

class ExactChangeEvent : public Event
{
public:
  bool exactChangeOnly;
  ExactChangeEvent(bool isExactOnly) : Event(TYPE_ID(ExactChangeEvent)), exactChangeOnly(isExactOnly) {}
};

class DispenseCoin : public Event
{
public:
  unsigned dispenserId;
  DispenseCoin(unsigned id) : Event(TYPE_ID(DispenseCoin)), dispenserId(id) {}
};