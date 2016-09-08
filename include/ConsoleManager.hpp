/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Handles events and logic that are assumed to be coming from the selection pad/
*    console. This makes sure purchases of items are legitimate before passing off a dispense
*    requests to the hardware and CurrencyManager.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
#pragma once
/*Realistically, options should be something like "A2" instead of an index value; however, since it isn't part
  of the stories, I will just assume something else will translate that for me and I will only pass index values*/
#include "json/value.h"
#include <istream>
#include <vector>

class Event;
class ConsoleManager
{
  struct Item
  {
    float price;
    unsigned quantity;
    Item(const Json::Value& value);
  };
  float currentAmountInserted;
  bool exactChangeOnly;
  std::vector<Item> items;
  void OnCurrencyEntered(const Event* e);
  void OnItemChosen(const Event* e);
  void OnCoinReturn(const Event* e);
  void OnExactChange(const Event* e);
  void RegisterEvents();
public:
  ConsoleManager(const Json::Value& jsonValue);
  
  float GetCurrentAmountInserted() { return currentAmountInserted; }
};
