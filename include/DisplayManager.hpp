/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Displaying the vending machine's state. (Returns string at GetDisplay())
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
#pragma once

#include "json/value.h"
#include <string>
#include <array>

class Event;
class DisplayManager
{
  std::string (DisplayManager::*currentDisplay)(void);
  float credit;
  bool isExactChange;
  float lastPriceInquiry;
  /**/
  std::string DisplayDefault(); /*INSERT COIN*/
  std::string DisplayCredit(); /*.25*/
  std::string DisplayExactChange(); /*EXACT CHANGE*/
  std::string DisplaySoldOut(); /*SOLD OUT*/
  std::string DisplayProductPrice(); /*PRICE 1.00*/
  std::string DisplayThanks(); /*THANK YOU*/
  /***/
  void OnCurrencyTaken(const Event* e);
  void OnDispenseItem(const Event* e);
  void OnExactChange(const Event* e);
  void OnNotEnoughMoney(const Event* e);
  void OnSoldOut(const Event* e);
  void OnDispenseChange(const Event* e);
  /***/
  void SetDefaultMessage();
  void RegisterEvents();
public:
  DisplayManager();
  std::string GetDisplay();
};

