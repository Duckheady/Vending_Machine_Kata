/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Defines events relating to currency. CurrencyInserted is an abstraction where
*    CurrencyTaken is currency that has a value assosiated with it.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
#pragma once

#include "Event.hpp"
#include "TypeId.hpp"
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
  TYPEID typeId;
  CurrencyTaken(float value, TYPEID id) : Event(TYPE_ID(CurrencyTaken)), valueOfCurrency(value), typeId(id) {}
};

class CurrencyRejected : public Event
{
public:
  CurrencyRejected() : Event(TYPE_ID(CurrencyRejected)) {}
};
