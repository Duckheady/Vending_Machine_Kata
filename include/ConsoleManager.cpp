#include "ConsoleManager.hpp"
#include "ConsoleEvents.hpp"
#include "DispenserEvents.hpp"
#include <json\reader.h>
#include "EventSystem.hpp"
#include "CurrencyEvents.hpp"

ConsoleManager::Item::Item(const Json::Value& value)
{
  //assert(value["cost"].isMember("cost"));
  //assert(value["quantity"].isMember("quantity"));
  price = value["cost"].asFloat();
  quantity = value["quantity"].asUInt();
}
ConsoleManager::ConsoleManager(std::istream& stream)
{
  currentAmountInserted = 0;
  exactChangeOnly = false;
  Json::Reader reader;
  Json::Value root;
  bool isGood = reader.parse(stream, root);
  if(isGood)
  {
    if(!root.isArray())
      throw;
    for(unsigned i = 0; i < root.size(); ++i)
    {
      items.push_back(Item(root[i]));
    }
  }
  RegisterEvents();
}

void ConsoleManager::RegisterEvents()
{
  RegisterClassCallback(ChooseItemEvent, *this, ConsoleManager, OnItemChosen);
  RegisterClassCallback(ChooseChangeReturn, *this, ConsoleManager, OnCoinReturn);
  RegisterClassCallback(CurrencyTaken, *this, ConsoleManager, OnCurrencyEntered);
  RegisterClassCallback(ExactChangeEvent, *this, ConsoleManager, OnExactChange);
}

void ConsoleManager::OnCurrencyEntered(const Event* e)
{
  currentAmountInserted += ((const CurrencyTaken*)e)->valueOfCurrency;
}

void ConsoleManager::OnItemChosen(const Event* e)
{
  unsigned index = ((const ChooseItemEvent*)e)->index;
  if(index < items.size())
  {
    if(items[index].quantity == 0)
    {
      SoldOutEvent soldOut(index);
      SendEvent(soldOut);
      return;
    }
    float itemPrice = items[index].price;
    /*To deal w/ floating point sillyness*/
    if(itemPrice == currentAmountInserted)
    {
      currentAmountInserted = 0;
      --items[index].quantity;
      DispenseItem item(index);
      SendEvent(item);
    }
    else if(itemPrice < currentAmountInserted)
    {
      currentAmountInserted -= itemPrice;
      DispenseItem item(index);
      float changeDue = currentAmountInserted;
      currentAmountInserted = 0;
      --items[index].quantity;
      SendEvent(item);
      if(!exactChangeOnly)
      {
        DispenseChange change(changeDue);
        SendEvent(change);
      }
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

void ConsoleManager::OnExactChange(const Event* e)
{
  exactChangeOnly = ((const ExactChangeEvent*)e)->exactChangeOnly;
}
