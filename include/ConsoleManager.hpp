#pragma once
/*Realistically, options should be something like "A2" instead of an index value; however, since it isn't part
  of the stories, I will just assume something else will translate that for me and I will only pass index values*/

#include <istream>
#include <vector>

class Event;
class ConsoleManager
{
  float currentAmountInserted;
  std::vector<float> itemPrices;
  void OnCurrencyEntered(const Event* e);
  void OnItemChosen(const Event* e);
  void OnCoinReturn(const Event* e);
  void RegisterEvents();
public:
  ConsoleManager(std::istream& stream);
  
  float GetCurrentAmountInserted() { return currentAmountInserted; }
};
