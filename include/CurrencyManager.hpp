#pragma once
#include <istream>
#include <vector>
#include "Currency.hpp"
#include "Event.hpp"

class CurrencyManager
{
  std::vector<CurrencyTemplate*> currencyTemplates;

  void OnCurrencyEntered(const Event* e);
public:
  CurrencyManager(std::istream& currencyDefinitions);
  ~CurrencyManager();
  void RegisterEvents();
};