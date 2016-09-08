/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Defining the interface for an event. (See EventSystem.hpp)
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
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
