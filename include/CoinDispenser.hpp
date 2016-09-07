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
