#include "ConsoleManager.hpp"
#include "ConsoleEvents.hpp"
#include "DispenserEvents.hpp"
#include <json\reader.h>
#include "EventSystem.hpp"
#include "CurrencyEvents.hpp"

ConsoleManager::ConsoleManager(std::istream& stream)
{
  currentAmountInserted = 0;
  Json::Reader reader;
  Json::Value root;
  bool isGood = reader.parse(stream, root);
  if(isGood)
  {
    if(!root.isArray())
      throw;
    for(unsigned i = 0; i < root.size(); ++i)
    {
      itemPrices.push_back(root[i]["cost"].asFloat());
    }
  }
  RegisterEvents();
}

void ConsoleManager::RegisterEvents()
{
  RegisterClassCallback(ChooseItemEvent, *this, ConsoleManager, OnItemChosen);
  RegisterClassCallback(ChooseChangeReturn, *this, ConsoleManager, OnCoinReturn);
  RegisterClassCallback(CurrencyTaken, *this, ConsoleManager, OnCurrencyEntered);
}

void ConsoleManager::OnCurrencyEntered(const Event* e)
{
  currentAmountInserted += ((const CurrencyTaken*)e)->valueOfCurrency;
}

void ConsoleManager::OnItemChosen(const Event* e)
{
  unsigned index = ((const ChooseItemEvent*)e)->index;
  if(index < itemPrices.size())
  {
    float itemPrice = itemPrices[index];
    /*To deal w/ floating point sillyness*/
    if(itemPrice == currentAmountInserted)
    {
      currentAmountInserted = 0;
      DispenseItem item(index);
      SendEvent(item);
    }
    else if(itemPrice < currentAmountInserted)
    {
      currentAmountInserted -= itemPrice;
      DispenseItem item(index);
      DispenseChange change(currentAmountInserted);
      currentAmountInserted = 0;
      SendEvent(item);
      SendEvent(change);
    }
    else
    {
      NotEnoughMoneyEvent noMoney(itemPrice);
      SendEvent(noMoney);
    }
  }
}

void ConsoleManager::OnCoinReturn(const Event*)
{
  if(currentAmountInserted > 0.0f)
  {
    DispenseChange change(currentAmountInserted);
    currentAmountInserted = 0;
    SendEvent(change);
  }
}