#pragma once
#include <fstream>
#include <vector>
#include "Currency.hpp"
#include "Event.hpp"

class CurrencyManager
{
  std::vector<CurrencyTemplate*> currencyTemplates;

  void OnCurrencyEntered(const Event* e);
  void RegisterEvents();
public:
  CurrencyManager(std::ifstream& currencyDefinitions);
  ~CurrencyManager();
  
};
