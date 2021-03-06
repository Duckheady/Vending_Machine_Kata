/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Triggers dispensers to dispense change to the customer. This handles
*    the making of change and also manages the global state of if insufficent coins are availible
*    to not require exact change.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/

#include "CoinDispenser.hpp"
#include "DispenserEvents.hpp"
#include "CurrencyEvents.hpp"
#include "EventSystem.hpp"
#include "Currency.hpp"
#include "FailureExceptions.hpp"
#include <algorithm>
#include <assert.h>

/*Dealing with annoying mantissas!*/
#define EPSILION .0001

CoinDispenser::Dispenser::Dispenser(const Json::Value& jsonValue)
{
  if(!jsonValue.isMember("value"))
    throw BadFileException();
  if(!jsonValue.isMember("numberOfCoins"))
    throw BadFileException();
  if(!jsonValue.isMember("dispenserId"))
    throw BadFileException();
  coinValue = jsonValue["value"].asFloat();
  numberOfCoins = jsonValue["numberOfCoins"].asUInt();
  dispenserId = jsonValue["dispenserId"].asUInt();
}

bool CoinDispenser::DispenserSorter(const CoinDispenser::Dispenser& lhs, const CoinDispenser::Dispenser& rhs)
{
  return lhs.coinValue > rhs.coinValue;
}

CoinDispenser::CoinDispenser(const Json::Value& currencies, const Json::Value& items) : totalChangeValue(0), maxItemValue(0)
{
  if(!currencies.isArray())
    throw BadFileException();
  if(!items.isArray())
    throw BadFileException();
  /*Initializing coin dispensers first and tallying money*/
  for(unsigned i = 0; i < currencies.size(); ++i)
  {
    if(!currencies[i].isMember("type"))
      throw BadFileException();
    if(currencies[i]["type"].asString() == "Coin")
    {
      dispensers.push_back(currencies[i]);
      totalChangeValue += (float)dispensers[i].numberOfCoins * dispensers[i].coinValue;
    }
  }
  /*Sort for the greedy algorthm found in OnDispenseChange(...)*/
  std::sort(dispensers.begin(), dispensers.end(), DispenserSorter);
  /*Finding highest cost and setting it.*/
  for(unsigned i = 0; i < items.size(); ++i)
  {
    if(!items[i].isMember("cost"))
      throw BadFileException();
    if(items[i]["cost"] > maxItemValue)
      maxItemValue = items[i]["cost"].asFloat();
  }
  /*Setting initial condition for exact change only*/
  isExactChange = totalChangeValue <= maxItemValue;
  ExactChangeEvent eChange(isExactChange);
  SendEvent(eChange);
  RegisterEvents();
}

void CoinDispenser::OnCurrencyTaken(const Event* e)
{
  const CurrencyTaken* cEvent =  (const CurrencyTaken*) e;
  if(cEvent->typeId == TYPE_ID(CoinTemplate))
  {
    for(unsigned i = 0; i < dispensers.size(); ++i)
    {
      if(dispensers[i].coinValue == cEvent->valueOfCurrency)
      {
        ++dispensers[i].numberOfCoins;
        totalChangeValue += dispensers[i].coinValue;
        VerifyExactChangeState();
      }
    }
  }
}

void CoinDispenser::OnDispenseChange(const Event* e)
{
  const DispenseChange* dEvent = (const DispenseChange*)e;
  float changeAmount = dEvent->changeAmount;
  if(changeAmount > totalChangeValue) /*Deals w/ problems of the paper currency or should it just skip cashing out?*/
    changeAmount = totalChangeValue;
  totalChangeValue -= changeAmount;
  /*Since it is sorted in decending order, we can iterate for
    a greedy algorithm approach*/
  for(unsigned i = 0; i < dispensers.size(); ++i)
  {
    if(changeAmount <= EPSILION)
      break;
    while(changeAmount + EPSILION >= (dispensers[i].coinValue - EPSILION) && dispensers[i].numberOfCoins > 0)
    {
      changeAmount -= dispensers[i].coinValue;
      --dispensers[i].numberOfCoins;
      /*Note: Not cache coeherant, might be worth refactoring later after profiling*/
      DispenseCoin coin(dispensers[i].dispenserId);
      SendEvent(coin);
    }
  }
  VerifyExactChangeState();
  assert(changeAmount <= EPSILION);
}

void CoinDispenser::VerifyExactChangeState()
{
  /*Only send state change if there is a change*/
  if(totalChangeValue <= maxItemValue && isExactChange == false)
    isExactChange = true;
  else if(totalChangeValue > maxItemValue && isExactChange == true)
    isExactChange = false;
  else
    return;
  ExactChangeEvent eChange(isExactChange);
  SendEvent(eChange);
}

void CoinDispenser::RegisterEvents()
{
  RegisterClassCallback(CurrencyTaken, *this, CoinDispenser, OnCurrencyTaken);
  RegisterClassCallback(DispenseChange, *this, CoinDispenser, OnDispenseChange);
}
