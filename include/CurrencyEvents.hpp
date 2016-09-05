#pragma once

#include "Event.hpp"
#include <json\value.h>

/*Bit of lazyness here just so I don't need to make getters/setters,
  Event system already guarentees constness.*/
  
/***In events***/
class CurrencyInserted : public Event
{
public:
  Json::Value coinData;
  CurrencyInserted(const Json::Value& coinValue) : Event(TYPE_ID(CurrencyInserted)), coinData(coinValue) {}
};

/***Out events***/
class CurrencyTaken : public Event
{
public:
  float valueOfCurrency;
  CurrencyTaken(float value) : Event(TYPE_ID(CurrencyTaken)), valueOfCurrency(value) {}
};

class CurrencyRejected : public Event
{
public:
  CurrencyRejected() : Event(TYPE_ID(CurrencyRejected)) {}
};
