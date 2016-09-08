/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Triggers dispensers to dispense change to the customer. This handles
*    the making of change and also manages the global state of if insufficent coins are availible
*    to not require exact change.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
#pragma once

#include <vector>
#include <json\value.h>

class Event;
class CoinDispenser
{
  struct Dispenser
  {
    float coinValue;
    unsigned numberOfCoins;
    unsigned dispenserId;
    Dispenser(const Json::Value& dispenser);
    //const Dispenser& operator=(const Dispenser& rhs);
  };
  /*Cost per coin, coins in dispenser*/
  std::vector<Dispenser> dispensers;
  float totalChangeValue;
  float maxItemValue;
  bool isExactChange;
  void OnCurrencyTaken(const Event* e);
  void OnDispenseChange(const Event* e);
  void VerifyExactChangeState();
  void RegisterEvents();
  static bool DispenserSorter(const Dispenser& lhs, const Dispenser& rhs);
public:
  CoinDispenser(const Json::Value& currencies, const Json::Value& items);
  float GetTotalChangeValue() { return totalChangeValue; }
  float GetMaxItemValue() { return maxItemValue; }
};
