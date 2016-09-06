#pragma once
#include <fstream>
#include <vector>
#include "Currency.hpp"
#include "Event.hpp"

class CurrencyManager
{
  std::vector<CurrencyTemplate*> currencyTemplates;

  void OnCurrencyEntered(const Event* e);
public:
  CurrencyManager(std::ifstream& currencyDefinitions);
  ~CurrencyManager();
  void RegisterEvents();
};