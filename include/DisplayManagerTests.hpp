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

class DisplayManagerTests : public testing::Test
{
protected:
  DisplayManager* dManager;

  virtual ~DisplayManagerTests() {}
  virtual void SetUp()
  {
    dManager = new DisplayManager();
  }
  virtual void TearDown()
  {
    delete dManager;
    dManager = nullptr;
    EventSystem::Teardown();
  }
};

TEST_F(DisplayManagerTests, DefaultCurrencyTakenTest)
{
  /*Commented blocks are in test fixture*/
  //DisplayManager* dManager = new DisplayManager()
  CurrencyTaken currencyTaken(.25f, 0);
  CurrencyTaken currencyTaken2(.75f, 0);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
  SendEvent(currencyTaken2);
  EXPECT_EQ(dManager->GetDisplay(), "1.00");
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager->GetDisplay(), "1.25");

  //delete dManager;
  //EventSystem::Teardown();
}

TEST_F(DisplayManagerTests, DispenseItemTest)
{
  CurrencyTaken currencyTaken(.25f, 0);
  DispenseItem dispenseItem(2);
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
  SendEvent(dispenseItem);
  EXPECT_EQ(dManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(DisplayManagerTests, ThanksMessageTest)
{
  CurrencyTaken currencyTaken(.25f, 0);
  DispenseItem dispenseItem(2);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager->GetDisplay(), "0.50");
  SendEvent(dispenseItem);
  EXPECT_EQ(dManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(DisplayManagerTests, DisplayPriceFromDefaultTest)
{
  NotEnoughMoneyEvent nmEvent(.80f);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(nmEvent);
  EXPECT_EQ(dManager->GetDisplay(), std::string(PRICE_MESSAGE) + "0.80");
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(DisplayManagerTests, DisplayPriceFromInsertedCoinsTest)
{
  CurrencyTaken currencyTaken(.25f, 0);
  NotEnoughMoneyEvent nmEvent(.80f);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
  SendEvent(nmEvent);
  EXPECT_EQ(dManager->GetDisplay(), std::string(PRICE_MESSAGE) + "0.80");
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
}

/*Not defined in the project stories, but everything else allows for change to
    be different, so why not here too? It hurts nothing*/
TEST_F(DisplayManagerTests, DisplayRemainingChangeAfterDispense)
{
  CurrencyTaken currencyTaken(.25f, 0);
  DispenseChange dispenseChange(.25f);
  DispenseItem dispenseItem(2);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(currencyTaken); /*.25*/
  SendEvent(currencyTaken); /*.50*/
  SendEvent(dispenseChange); /*Only 0.25*/
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
  SendEvent(dispenseItem);
  EXPECT_EQ(dManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(DisplayManagerTests, ExactChangeTest1)
{
  CurrencyTaken currencyTaken(.25f, 0);
  DispenseItem dispenseItem(2);
  ExactChangeEvent ecEvent(true);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(ecEvent); /*Now only exact change*/
  EXPECT_EQ(dManager->GetDisplay(), EXACT_CHANGE_MESSAGE);
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
  SendEvent(dispenseItem);
  EXPECT_EQ(dManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager->GetDisplay(), EXACT_CHANGE_MESSAGE);
  ecEvent.exactChangeOnly = false; /*Now normal*/
  SendEvent(ecEvent);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(DisplayManagerTests, ExactChangeTest2)
{
  CurrencyTaken currencyTaken(.25f, 0);
  DispenseItem dispenseItem(2);
  ExactChangeEvent ecEvent(true);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(ecEvent);
  EXPECT_EQ(dManager->GetDisplay(), EXACT_CHANGE_MESSAGE);
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
  ecEvent.exactChangeOnly = false;
  SendEvent(ecEvent);
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
  SendEvent(dispenseItem)
  EXPECT_EQ(dManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(DisplayManagerTests, SoldOutTest1)
{
  SoldOutEvent soEvent(3);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(soEvent);
  EXPECT_EQ(dManager->GetDisplay(), SOLD_OUT_MESSAGE);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(DisplayManagerTests, SoldOutTest2)
{
  CurrencyTaken currencyTaken(.25f, 0);
  SoldOutEvent soEvent(3);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(currencyTaken);
  EXPECT_EQ(dManager->GetDisplay(), "0.25");
  SendEvent(soEvent);
  EXPECT_EQ(dManager->GetDisplay(), SOLD_OUT_MESSAGE);
}

TEST_F(DisplayManagerTests, SoldOutIntoDispenseMessage)
{
  SoldOutEvent soEvent(3);
  DispenseItem dispenseItem(2);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(soEvent);
  EXPECT_EQ(dManager->GetDisplay(), SOLD_OUT_MESSAGE);
  SendEvent(dispenseItem)
  EXPECT_EQ(dManager->GetDisplay(), THANKS_MESSAGE);
}

TEST_F(DisplayManagerTests, SoldOutIntoPriceMessage)
{
  NotEnoughMoneyEvent nmEvent(.80f);
  SoldOutEvent soEvent(3);
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
  /***/
  SendEvent(soEvent);
  EXPECT_EQ(dManager->GetDisplay(), SOLD_OUT_MESSAGE);
  SendEvent(nmEvent);
  EXPECT_EQ(dManager->GetDisplay(), std::string(PRICE_MESSAGE) + "0.80");
  EXPECT_EQ(dManager->GetDisplay(), DEFAULT_MESSAGE);
}

