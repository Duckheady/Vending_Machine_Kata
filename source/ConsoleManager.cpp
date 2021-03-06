/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Handles events and logic that are assumed to be coming from the selection pad/
*    console. This makes sure purchases of items are legitimate before passing off a dispense
*    requests to the hardware and CurrencyManager.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
#include "ConsoleManager.hpp"
#include "ConsoleEvents.hpp"
#include "DispenserEvents.hpp"
#include <json\reader.h>
#include "EventSystem.hpp"
#include "CurrencyEvents.hpp"
#include "FailureExceptions.hpp"

ConsoleManager::Item::Item(const Json::Value& value)
{
  if(!value.isMember("cost"))
    throw BadFileException();
  if(!value.isMember("quantity"))
    throw BadFileException();
  price = value["cost"].asFloat();
  quantity = value["quantity"].asUInt();
}
ConsoleManager::ConsoleManager(const Json::Value& root)
{
  currentAmountInserted = 0;
  exactChangeOnly = false;
  if(!root.isArray())
    throw BadFileException();
  for(unsigned i = 0; i < root.size(); ++i)
    items.push_back(Item(root[i]));
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
