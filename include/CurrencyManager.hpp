/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Handles hardware events that send abstract currency and assigns a value and type
*    to the abstraction. Alternatively, if it doesn't find a template, it rejects the currency entirely.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
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
  CurrencyManager(const Json::Value& currencyDefinitions);
  ~CurrencyManager();
  
};
