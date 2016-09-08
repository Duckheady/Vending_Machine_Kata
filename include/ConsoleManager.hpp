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
