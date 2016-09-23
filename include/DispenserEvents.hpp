/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Events relating to dispensary. While CoinDispenser uses alot of this
*    there is some overlap to real system out events like DispenseItem and DispenseCoin.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
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

