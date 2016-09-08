#pragma once

#include "Event.hpp"

class ChooseItemEvent : public Event
{
public:
  unsigned index;
  ChooseItemEvent(unsigned choice) : Event(TYPE_ID(ChooseItemEvent)), index(choice) {}
};

class ChooseChangeReturn : public Event
{
public:
  ChooseChangeReturn() : Event(TYPE_ID(ChooseChangeReturn)) {}
};

class NotEnoughMoneyEvent : public Event
{
public:
  float priceOfItem;
  NotEnoughMoneyEvent(float price) : Event(TYPE_ID(NotEnoughMoneyEvent)), priceOfItem(price) {}
};

class SoldOutEvent : public Event
{
public:
  unsigned indexSoldOut;
  SoldOutEvent(unsigned soldOutIndex) : Event(TYPE_ID(SoldOutEvent)), indexSoldOut(soldOutIndex) {}
};
