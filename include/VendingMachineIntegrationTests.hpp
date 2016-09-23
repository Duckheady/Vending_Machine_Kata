/*********************************************************************************
*  Author: Nicholas Louks
*  File Brief: Tests integration between the ConsoleManager, CurrencyManager
*    DisplayManger, and CoinDispenser. See below comment for more on expected input and output
*    events in addition to each modules file or the readme for information on thier responsiblities.
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
#include "CurrencyManager.hpp"
#include "DisplayManager.hpp"
#include "ConsoleManager.hpp"
#include "CoinDispenser.hpp"

/******************************
To be completely clear, there are three input events which represent the three inputs that the users can input.
These tests are to simulate hardware pulses to/from the system. 
In events are to be generated from the hardware / adaptor. Out events are to be sent to the hardware / adaptor. 
The in-events are:
  CurrencyInserted ~ (CurrencyEvents.hpp) -- Simulates the event of passing properties of money being inserted into the machine.
  ChooseChangeReturn ~ (ConsoleEvents.hpp) -- Simulates the event the user pressing the change return once.
  ChooseItemEvent ~ (ConsoleEvents.hpp) -- Simulates the event of the user selecting an item to purchace in the vending machine.
The out-events are:
  DispenseItem ~ (DispenserEvents.hpp) -- Output to dispense the item <itemIndex>.
  DispenseCoin ~ (DispenserEvents.hpp) -- Output to dispense one coin in dispenser <dispenserId>.
  CurrencyTaken ~ (CurrencyEvents.hpp) -- Take the last coin sent.
  CurrencyRejected ~ (CurrencyEvents.hpp) -- Reject last coin sent.
Additonally, as per stories, there is a display which, when called should return the correct string based on the state of the machine.
This function is DisplayManager::GetDisplay().
******************************/

class VendingMachineIntegrationTests : public testing::Test
{
 
protected:
  Json::Value dime, quarter, nickel;
  Event* changeReturnEvent;
  Event* chooseItem0;
  Event* chooseItem1;
  Event* chooseItem2;
  Event* sendDime;
  Event* sendQuarter;
  Event* sendNickel;
  CurrencyManager* currencyManager;
  ConsoleManager* consoleManager;
  DisplayManager* displayManager;
  CoinDispenser* coinDispenser;
  /***/
  bool lastCoinWasAccepted;
  unsigned dispensersTriggered[3];
  unsigned lastItemDispensed;

  void SendCurrencyInsertedWithNoise(Json::Value testValue, const std::string& testProperty, float noise)
  {
    testValue[testProperty] = testValue[testProperty].asFloat() + noise;
    CurrencyInserted insertCoin(testValue);
    SendEvent(insertCoin);
  }

  void ResetDispenserCapture()
  {
    for(unsigned i = 0; i < 3; ++i)
      dispensersTriggered[i] = 0;
  }

  void OnDispenseItem(const Event* e)
  {
    lastItemDispensed = ((const DispenseItem*)e)->itemIndex;
  }
  void OnDispenseCoin(const Event* e)
  {
    ++dispensersTriggered[((const DispenseCoin*)e)->dispenserId];
  }
  void OnCurrencyTaken(const Event*)
  {
    lastCoinWasAccepted = true;
  }
  void OnCurrencyRejected(const Event*)
  {
    lastCoinWasAccepted = false;
  }
  void SetupJsonValue(std::ifstream& currencyStream, std::ifstream& itemStream, Json::Value& currencyRoot, Json::Value& itemRoot)
  {
    Json::Reader streamReader;
    ASSERT_TRUE(streamReader.parse(currencyStream, currencyRoot));
    ASSERT_TRUE(streamReader.parse(itemStream, itemRoot));
  }
  void SetupManagers(const Json::Value& currencyRoot, const Json::Value& itemRoot)
  {
    displayManager = new DisplayManager;
    coinDispenser = new CoinDispenser(currencyRoot, itemRoot);
    consoleManager = new ConsoleManager(itemRoot);
    currencyManager = new CurrencyManager(currencyRoot);
  }

  virtual void SetupParametersAndEvents()
  {
    /**Defining expected correct coins**/
    dime["radius"] = 0.12f;
    dime["weight"] = 2.25f;
    quarter["radius"] = 0.5f;
    quarter["weight"] = 12.5f;
    nickel["radius"] = 0.3f;
    nickel["weight"] = 4.5f;
    /*Setting up some basic input events*/
    changeReturnEvent = new ChooseChangeReturn();
    chooseItem0 = new ChooseItemEvent(0);
    chooseItem1 = new ChooseItemEvent(1);
    chooseItem2 = new ChooseItemEvent(2);
    sendDime = new CurrencyInserted(dime);
    sendQuarter = new CurrencyInserted(quarter);
    sendNickel = new CurrencyInserted(nickel);
    /***/
    lastCoinWasAccepted = false;
    ResetDispenserCapture();
    lastItemDispensed = -1;
    /**Register capture events!**/
    RegisterClassCallback(DispenseItem, *this, VendingMachineIntegrationTests, OnDispenseItem);
    RegisterClassCallback(DispenseCoin, *this, VendingMachineIntegrationTests, OnDispenseCoin);
    RegisterClassCallback(CurrencyTaken, *this, VendingMachineIntegrationTests, OnCurrencyTaken);
    RegisterClassCallback(CurrencyRejected, *this, VendingMachineIntegrationTests, OnCurrencyRejected);
  }
  virtual void SetUp()
  {
    std::ifstream currencyStream("testCurrencies.json");
    std::ifstream itemStream("testItems.json");
    Json::Value currencyRoot, itemRoot;
    SetupJsonValue(currencyStream, itemStream, currencyRoot, itemRoot);
    SetupManagers(currencyRoot, itemRoot);
    SetupParametersAndEvents();
  }

  virtual void TearDown()
  {
    delete sendNickel;
    delete sendQuarter;
    delete sendDime;
    delete chooseItem2;
    delete chooseItem1;
    delete chooseItem0;
    delete changeReturnEvent;
    delete currencyManager;
    delete consoleManager;
    delete coinDispenser;
    delete displayManager;
    /***/
    sendNickel = sendQuarter = sendDime = chooseItem2 = chooseItem1 = chooseItem0 = changeReturnEvent = nullptr;
    currencyManager = nullptr;
    consoleManager = nullptr;
    coinDispenser = nullptr;
    displayManager = nullptr;
    EventSystem::Teardown();
  }
};

/*What is happening with SendEvent(*sendDime):
  CurrencyInserted event is sent to the EventManager.
  Those signed up for the events (i.e. CurrencyManager) pick out the data.
  In this case, CurrencyManager validates the coin, and sends another event CurrencyTaken (CurrencyEvents.hpp).
  Those signed up for this event (i.e. ConsoleManager, DisplayManager, and CoinDispenser) then react and return.
  Program flow then continues at the line following SendEvent(*sendDime).
*/
/*Other events follow the same idea, all of them unaware of the other listeners*/
TEST_F(VendingMachineIntegrationTests, SimplePurchase)
{
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
  SendEvent(*sendDime); /*Credit: .10*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.10");
  SendEvent(*sendQuarter); /*Credit: .35*/
  SendEvent(*sendQuarter); /*Credit: .60*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.60");
  SendEvent(*sendNickel); /*Credit: .65*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.65");
  SendEvent(*chooseItem2);
  EXPECT_EQ(lastItemDispensed, 2u); /*Should have vended item 2*/
  EXPECT_EQ(dispensersTriggered[0], 0u); /*No dispensers should have been triggered*/
  EXPECT_EQ(dispensersTriggered[1], 0u);
  EXPECT_EQ(dispensersTriggered[2], 0u);
  EXPECT_EQ(displayManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(VendingMachineIntegrationTests, SimplePurchaseWithChange)
{
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
  SendEvent(*sendDime); /*Credit: .10*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.10");
  SendEvent(*sendQuarter); /*Credit: .35*/
  SendEvent(*sendQuarter); /*Credit: .60*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.60");
  SendEvent(*sendDime); /*Credit: .70*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.70");
  SendEvent(*chooseItem2);
  EXPECT_EQ(lastItemDispensed, 2u); /*Should have vended item 2*/
  EXPECT_EQ(dispensersTriggered[0], 0u); /*Nickel's dispenser should have been triggered once*/
  EXPECT_EQ(dispensersTriggered[1], 0u);
  EXPECT_EQ(dispensersTriggered[2], 1u);
  EXPECT_EQ(displayManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(VendingMachineIntegrationTests, DisplayPriceThanPurchase)
{
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
  SendEvent(*chooseItem0);
  EXPECT_EQ(lastItemDispensed, (unsigned)-1); /*No item vended*/
  EXPECT_EQ(displayManager->GetDisplay(), "PRICE 1.00"); /*Item0's cost is 1.00*/
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
  SendEvent(*sendDime); /*Credit: .10*/
  SendEvent(*sendQuarter); /*Credit: .35*/
  SendEvent(*sendQuarter); /*Credit: .60*/
  SendEvent(*sendDime); /*Credit: .70*/
  SendEvent(*sendNickel); /*Credit: .75*/
  SendEvent(*sendNickel); /*Credit: .80*/
  SendEvent(*sendDime); /*Credit: .90*/
  SendEvent(*sendDime); /*Credit: 1.00*/
  EXPECT_EQ(displayManager->GetDisplay(), "1.00");
  SendEvent(*chooseItem0);
  EXPECT_EQ(lastItemDispensed, 0u); /*Should have vended item 0*/
  EXPECT_EQ(displayManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(VendingMachineIntegrationTests, CoinReturnTest)
{
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
  SendEvent(*sendDime); /*Credit: .10*/
  SendEvent(*sendQuarter); /*Credit: .35*/
  SendEvent(*sendQuarter); /*Credit: .60*/
  SendEvent(*sendDime); /*Credit: .70*/
  SendEvent(*sendNickel); /*Credit: .75*/
  SendEvent(*sendNickel); /*Credit: .80*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.80");
  SendEvent(*changeReturnEvent);
  EXPECT_EQ(dispensersTriggered[0], 0u); /*0 dimes*/ 
  EXPECT_EQ(dispensersTriggered[1], 3u); /*3 quarters: .75*/
  EXPECT_EQ(dispensersTriggered[2], 1u); /*1 nickel: .05*/
  EXPECT_EQ(lastItemDispensed, (unsigned)-1); /*Should have not vended anything*/
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
}

TEST_F(VendingMachineIntegrationTests, CoinReturnTestCoinVariance)
{
  const float goodVariance = .0001f;
  const float badVariance = .1f;
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
  SendCurrencyInsertedWithNoise(dime, "weight", goodVariance); /*Credit: .10*/
  EXPECT_TRUE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.10");
  SendCurrencyInsertedWithNoise(dime, "weight", -goodVariance); /*Credit: .20*/
  EXPECT_TRUE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.20");
  SendCurrencyInsertedWithNoise(quarter, "weight", badVariance); /*Credit: .20*/
  EXPECT_FALSE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.20");
  SendCurrencyInsertedWithNoise(quarter, "radius", -badVariance); /*Credit: .20*/
  EXPECT_FALSE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.20");
  SendCurrencyInsertedWithNoise(quarter, "weight", goodVariance); /*Credit: .45*/
  EXPECT_TRUE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.45");
  SendEvent(*changeReturnEvent);
  EXPECT_EQ(dispensersTriggered[0], 2u); /*2 dimes: .20*/ 
  EXPECT_EQ(dispensersTriggered[1], 1u); /*1 quarters: .25*/
  EXPECT_EQ(dispensersTriggered[2], 0u); /*0 nickel: 0.00*/
  EXPECT_EQ(lastItemDispensed, (unsigned)-1); /*Should have not vended anything*/
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE);
}

/*********************************************************************************************************************/
/*Quick setup of low resource environment, mostly the same as above*/
class LowResourceVendingMachineIntegrationTests : public VendingMachineIntegrationTests
{
protected:
  virtual void SetUp()
  {
    std::ifstream currencyStream("lowCoinCurrencies.json");
    std::ifstream itemStream("lowQuantityTestItems.json");
    Json::Value currencyRoot, itemRoot;
    SetupJsonValue(currencyStream, itemStream, currencyRoot, itemRoot);
    SetupManagers(currencyRoot, itemRoot);
    SetupParametersAndEvents();
  }
};
/*Only 6 dimes and 1 nickel. (.65 total)*/
/*Only 2 cokes (1.00), 1 chips (.50), 0 candy (.65)*/

TEST_F(LowResourceVendingMachineIntegrationTests, SimplePurchase)
{
  EXPECT_EQ(displayManager->GetDisplay(), EXACT_CHANGE_MESSAGE);
  SendEvent(*sendDime); /*Credit: .10*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.10");
  SendEvent(*sendQuarter); /*Credit: .35*/
  SendEvent(*sendDime); /*Credit: .45*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.45");
  SendEvent(*sendNickel); /*Credit: .50*/
  EXPECT_EQ(displayManager->GetDisplay(), "0.50");
  SendEvent(*chooseItem1);
  EXPECT_EQ(lastItemDispensed, 1u); /*Should have vended item 1*/
  EXPECT_EQ(dispensersTriggered[0], 0u); /*No dispensers should have been triggered*/
  EXPECT_EQ(dispensersTriggered[1], 0u);
  EXPECT_EQ(dispensersTriggered[2], 0u);
  EXPECT_EQ(displayManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE); /*Now have 1.10 so should be default*/
}

TEST_F(LowResourceVendingMachineIntegrationTests, PurchaseChipsTwice)
{
  EXPECT_EQ(displayManager->GetDisplay(), EXACT_CHANGE_MESSAGE);
  SendEvent(*sendDime); /*Credit: .10*/
  SendEvent(*sendQuarter); /*Credit: .35*/
  SendEvent(*sendDime); /*Credit: .45*/
  SendEvent(*sendNickel); /*Credit: .50*/
  SendEvent(*chooseItem1);
  EXPECT_EQ(lastItemDispensed, 1u); /*Should have vended item 1*/
  EXPECT_EQ(dispensersTriggered[0], 0u); /*No dispensers should have been triggered*/
  EXPECT_EQ(dispensersTriggered[1], 0u);
  EXPECT_EQ(dispensersTriggered[2], 0u);
  EXPECT_EQ(displayManager->GetDisplay(), THANKS_MESSAGE);
  EXPECT_EQ(displayManager->GetDisplay(), DEFAULT_MESSAGE); /*Now have 1.10 so should be default*/
  /***Try again***/
  lastItemDispensed = -1;
  SendEvent(*sendQuarter); /*Credit: .25*/
  SendEvent(*sendQuarter); /*Credit: .50*/
  SendEvent(*chooseItem1);
  EXPECT_EQ(lastItemDispensed, (unsigned)-1); /*Should have vended nothing*/
  EXPECT_EQ(dispensersTriggered[0], 0u); /*No dispensers should have been triggered*/
  EXPECT_EQ(dispensersTriggered[1], 0u);
  EXPECT_EQ(dispensersTriggered[2], 0u);
  EXPECT_EQ(displayManager->GetDisplay(), SOLD_OUT_MESSAGE);
  EXPECT_EQ(displayManager->GetDisplay(), "0.50");
  SendEvent(*changeReturnEvent);
  EXPECT_EQ(dispensersTriggered[0], 0u); 
  EXPECT_EQ(dispensersTriggered[1], 2u); /*2 Quarters*/
  EXPECT_EQ(dispensersTriggered[2], 0u);
}

TEST_F(LowResourceVendingMachineIntegrationTests, VariantCurrencyTest)
{
  const float goodVariance = .0001f;
  const float badVariance = .1f;
  EXPECT_EQ(displayManager->GetDisplay(), EXACT_CHANGE_MESSAGE);
  SendCurrencyInsertedWithNoise(dime, "radius", -badVariance); /*Credit: .00*/
  EXPECT_FALSE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), EXACT_CHANGE_MESSAGE);
  SendCurrencyInsertedWithNoise(dime, "radius", goodVariance); /*Credit: .10*/
  EXPECT_TRUE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.10");
  SendCurrencyInsertedWithNoise(dime, "radius", -goodVariance); /*Credit: .20*/
  EXPECT_TRUE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.20");
  SendCurrencyInsertedWithNoise(quarter, "radius", badVariance); /*Credit: .20*/
  EXPECT_FALSE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.20");
  SendCurrencyInsertedWithNoise(quarter, "weight", -badVariance); /*Credit: .20*/
  EXPECT_FALSE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.20");
  SendCurrencyInsertedWithNoise(quarter, "radius", goodVariance); /*Credit: .45*/
  EXPECT_TRUE(lastCoinWasAccepted);
  EXPECT_EQ(displayManager->GetDisplay(), "0.45");
  SendEvent(*changeReturnEvent);
  EXPECT_EQ(dispensersTriggered[0], 2u); /*2 dimes: .20*/ 
  EXPECT_EQ(dispensersTriggered[1], 1u); /*1 quarters: .25*/
  EXPECT_EQ(dispensersTriggered[2], 0u); /*0 nickel: 0.00*/
  EXPECT_EQ(lastItemDispensed, (unsigned)-1); /*Should have not vended anything*/
  EXPECT_EQ(displayManager->GetDisplay(), EXACT_CHANGE_MESSAGE);
}

