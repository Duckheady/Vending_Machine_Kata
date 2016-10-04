/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Displaying the vending machine's state. (Returns string at GetDisplay())
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/

#include "DisplayManager.hpp"
#include "CurrencyEvents.hpp"
#include "ConsoleEvents.hpp"
#include "DispenserEvents.hpp"
#include "EventSystem.hpp"
#include "DisplayDefines.hpp"
#include <sstream>
#include <iomanip>



/*While hard coding is not the best choice, putting in an entire system
  to recognize symbols and then assigning values to those symbols also seems
  like overkill with a project of this scope. The strings will be minimal, and
  there will not be many of them, and the logic need to be appended anyways for each new one
  the only thing we would gain is not recompiling for a new preexisting string, but then that has
  issues in things like displaying credit.*/
DisplayManager::DisplayManager() : credit(0), isExactChange(false), lastPriceInquiry(0), isOutOfOrder(false)
{
  currentDisplay = &DisplayManager::DisplayDefault;
  RegisterEvents();
}

std::string DisplayManager::DisplayDefault()
{
  return DEFAULT_MESSAGE;
}

std::string DisplayManager::DisplayCredit()
{
  std::stringstream ans; /*Easy, dirty way...*/
  ans << std::fixed <<std::setprecision(2) << credit;
  return ans.str();
}

std::string DisplayManager::DisplayExactChange()
{
  return EXACT_CHANGE_MESSAGE;
}

std::string DisplayManager::DisplaySoldOut()
{
  SetDefaultMessage();
  return SOLD_OUT_MESSAGE;
}

std::string DisplayManager::DisplayProductPrice()
{
  std::stringstream ans; /*Easy, dirty way...*/
  ans << PRICE_MESSAGE << std::fixed << std::setprecision(2) << lastPriceInquiry;
  SetDefaultMessage();
  return ans.str();
}

std::string DisplayManager::DisplayThanks()
{
  SetDefaultMessage();
  return THANKS_MESSAGE;
}

/******/

/*Sets messages to the correct persistant message, primarily used after pulse messages like SoldOut*/
void DisplayManager::SetDefaultMessage()
{
  if(credit > 0.0f)
    currentDisplay = &DisplayManager::DisplayCredit;
  else if(isExactChange)
    currentDisplay = &DisplayManager::DisplayExactChange;
  else
    currentDisplay = &DisplayManager::DisplayDefault;
}

/******/

void DisplayManager::OnCurrencyTaken(const Event* e)
{
  const CurrencyTaken* cEvent = (const CurrencyTaken*)e;
  credit += cEvent->valueOfCurrency;
  SetDefaultMessage();
}

void DisplayManager::OnDispenseItem(const Event*)
{
  credit = 0.0f;
  currentDisplay = &DisplayManager::DisplayThanks;
}

void DisplayManager::OnExactChange(const Event* e)
{
  const ExactChangeEvent* eEvent = (const ExactChangeEvent*)e;
  isExactChange = eEvent->exactChangeOnly;
  SetDefaultMessage();
}

void DisplayManager::OnNotEnoughMoney(const Event* e)
{
  const NotEnoughMoneyEvent* nmEvent = (const NotEnoughMoneyEvent*)e;
  lastPriceInquiry = nmEvent->priceOfItem;
  currentDisplay = &DisplayManager::DisplayProductPrice;
}

void DisplayManager::OnSoldOut(const Event*)
{
  currentDisplay = &DisplayManager::DisplaySoldOut;
}

void DisplayManager::OnDispenseChange(const Event* e)
{
  if(credit == 0.0f)
    return;
  const DispenseChange* dEvent = (const DispenseChange*)e;
  credit -= dEvent->changeAmount;
  SetDefaultMessage();
}
/***/
void DisplayManager::RegisterEvents()
{
  RegisterClassCallback(CurrencyTaken, *this, DisplayManager, OnCurrencyTaken);
  RegisterClassCallback(DispenseItem, *this, DisplayManager, OnDispenseItem);
  RegisterClassCallback(NotEnoughMoneyEvent, *this, DisplayManager, OnNotEnoughMoney);
  RegisterClassCallback(ExactChangeEvent, *this, DisplayManager, OnExactChange);
  RegisterClassCallback(SoldOutEvent, *this, DisplayManager, OnSoldOut);
  RegisterClassCallback(DispenseChange, *this, DisplayManager, OnDispenseChange)
}
/***/
std::string DisplayManager::GetDisplay()
{
  if(isOutOfOrder)
    return std::string(FAILURE_MESSAGE);
  return (this->*currentDisplay)();
}

void DisplayManager::SetOutOfOrder()
{
  isOutOfOrder = true;
}