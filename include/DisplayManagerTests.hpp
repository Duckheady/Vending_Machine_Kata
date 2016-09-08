/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Tests for the DisplayManager.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/

#pragma once
#include "gtest/gtest.h"
#include <json/reader.h>
#include "EventSystem.hpp"
#include "DisplayManager.hpp"
#include "ConsoleEvents.hpp"
#include "CurrencyEvents.hpp"
#include "DispenserEvents.hpp"
#include "DisplayDefines.hpp"

TEST(DisplayManagerTests, DefaultCurrencyTakenTest)
{
  CurrencyTaken currencyTaken(.25f, 0);
  CurrencyTaken currencyTaken2(.75f, 0);
  DisplayManager dManager;
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  SendEvent(currencyTaken2);
  EXPECT_EQ(dManager.GetDisplay(), "1.00");
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager.GetDisplay(), "1.25");
  /***/
  EventSystem::Teardown();
}

TEST(DisplayManagerTests, DispenseItemTest)
{
  CurrencyTaken currencyTaken(.25f, 0);
  DispenseItem dispenseItem(2);
  DisplayManager dManager;
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  SendEvent(dispenseItem);
  EXPECT_EQ(dManager.GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  /***/
  EventSystem::Teardown();
}

TEST(DisplayManagerTests, AdvancedDisplayTests)
{
  /*Events*/
  CurrencyTaken currencyTaken(.25f, 0);
  DispenseItem dispenseItem(2);
  DispenseChange dispenseChange(.25f);
  NotEnoughMoneyEvent nmEvent(.80f);
  ExactChangeEvent ecEvent(true);
  SoldOutEvent soEvent(3);
  /***/
  DisplayManager dManager;
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager.GetDisplay(), "0.50");
  SendEvent(dispenseItem);
  EXPECT_EQ(dManager.GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(nmEvent);
  EXPECT_EQ(dManager.GetDisplay(), std::string(PRICE_MESSAGE) + "0.80");
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  SendEvent(nmEvent);
  EXPECT_EQ(dManager.GetDisplay(), std::string(PRICE_MESSAGE) + "0.80");
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  /***/
  /*Not defined in the project stories, but everything else allows for change to
    be different, so why not here too? It hurts nothing*/
  SendEvent(dispenseChange);
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  SendEvent(currencyTaken);
  SendEvent(currencyTaken);
  SendEvent(dispenseChange);
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  SendEvent(dispenseItem);
  EXPECT_EQ(dManager.GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(ecEvent);
  EXPECT_EQ(dManager.GetDisplay(), EXACT_CHANGE_MESSAGE);
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  SendEvent(dispenseItem);
  EXPECT_EQ(dManager.GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager.GetDisplay(), EXACT_CHANGE_MESSAGE);
  ecEvent.exactChangeOnly = false;
  SendEvent(ecEvent);
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  /***/
  ecEvent.exactChangeOnly = true;
  SendEvent(ecEvent);
  EXPECT_EQ(dManager.GetDisplay(), EXACT_CHANGE_MESSAGE);
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  ecEvent.exactChangeOnly = false;
  SendEvent(ecEvent);
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  SendEvent(dispenseItem)
  EXPECT_EQ(dManager.GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(soEvent);
  EXPECT_EQ(dManager.GetDisplay(), SOLD_OUT_MESSAGE);
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager.GetDisplay(), "0.25");
  SendEvent(soEvent);
  EXPECT_EQ(dManager.GetDisplay(), SOLD_OUT_MESSAGE);
  SendEvent(dispenseItem)
  EXPECT_EQ(dManager.GetDisplay(), THANKS_MESSAGE);
  SendEvent(soEvent);
  EXPECT_EQ(dManager.GetDisplay(), SOLD_OUT_MESSAGE);
  SendEvent(nmEvent);
  EXPECT_EQ(dManager.GetDisplay(), std::string(PRICE_MESSAGE) + "0.80");
  EXPECT_EQ(dManager.GetDisplay(), DEFAULT_MESSAGE);
  /***/
  EventSystem::Teardown();
}