/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Tests for the Coindispenser.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
#pragma once

#include "gtest/gtest.h"
#include "EventSystem.hpp"
#include <fstream>
#include <json\reader.h>
#include "CoinDispenser.hpp"
#include "ConsoleEvents.hpp"
#include "CurrencyEvents.hpp"
#include "DispenserEvents.hpp"
#include "Currency.hpp"

namespace CoinDispenserTests
{
  void TestLoad(std::ifstream& currencyStream, std::ifstream& itemStream, Json::Value& currencyValue, Json::Value& itemValue)
  {
    ASSERT_TRUE(currencyStream.good());
    ASSERT_TRUE(itemStream.good());
    Json::Reader streamReader;
    bool isGood = streamReader.parse(currencyStream, currencyValue);
    ASSERT_TRUE(isGood);
    ASSERT_TRUE(currencyValue.isArray());
    isGood = streamReader.parse(itemStream, itemValue);
    ASSERT_TRUE(isGood);
    ASSERT_TRUE(itemValue.isArray());
  }

  void ResetArray(std::vector<unsigned>& coinsDispensed)
  {
    for(unsigned i = 0; i < coinsDispensed.size(); ++i)
      coinsDispensed[i] = 0;
  }
}

/*THIS TEST DEPENDS ON THE INDIVIDUAL VALUES OF THE DATA!*/
/*dispenser0 == .10, dispenser1 == .25, dispenser2 == .05*/
TEST(CoinDispenserTests, SimpleDispenseTests)
{
  Json::Value currencyRoot, itemRoot;
  std::ifstream currencyStream("testCurrencies.json");
  std::ifstream itemStream("testItems.json");
  CoinDispenserTests::TestLoad(currencyStream, itemStream, currencyRoot, itemRoot);
  ASSERT_EQ(currencyRoot.size(), 3);
  ASSERT_FLOAT_EQ(currencyRoot[0]["value"].asFloat(), .10f);
  ASSERT_FLOAT_EQ(currencyRoot[1]["value"].asFloat(), .25f);
  ASSERT_FLOAT_EQ(currencyRoot[2]["value"].asFloat(), .05f);
  std::vector<unsigned> coinsDispensed;
  coinsDispensed.resize(currencyRoot.size());
  RegisterNormalCallback
    (DispenseCoin, 
    /*Captures DispenseCoin event*/
    [&coinsDispensed] (const Event* gEvent) 
      { 
        const DispenseCoin* iEvent = (const DispenseCoin*) gEvent;
        ++coinsDispensed[iEvent->dispenserId];
      }
    );
  
  
  CoinDispenser cDispenser(currencyRoot, itemRoot);
  DispenseChange dispenseChange(.50);
  /*****Begin tests*****/  
  SendEvent(dispenseChange);
  EXPECT_EQ(coinsDispensed[0], 0); /*.10*/
  EXPECT_EQ(coinsDispensed[1], 2); /*.25*/
  EXPECT_EQ(coinsDispensed[2], 0); /*.05*/
  CoinDispenserTests::ResetArray(coinsDispensed);
  /***/  
  dispenseChange = .65f;
  SendEvent(dispenseChange);
  EXPECT_EQ(coinsDispensed[0], 1);
  EXPECT_EQ(coinsDispensed[1], 2);
  EXPECT_EQ(coinsDispensed[2], 1);
  CoinDispenserTests::ResetArray(coinsDispensed);
  /***/
  dispenseChange = .20f;
  SendEvent(dispenseChange);
  EXPECT_EQ(coinsDispensed[0], 2);
  EXPECT_EQ(coinsDispensed[1], 0);
  EXPECT_EQ(coinsDispensed[2], 0);
  CoinDispenserTests::ResetArray(coinsDispensed);
  /***/
  dispenseChange = .05f;
  SendEvent(dispenseChange);
  EXPECT_EQ(coinsDispensed[0], 0);
  EXPECT_EQ(coinsDispensed[1], 0);
  EXPECT_EQ(coinsDispensed[2], 1);
  CoinDispenserTests::ResetArray(coinsDispensed);
  /***/
  EventSystem::Teardown();
}

TEST(CoinDispenserTests, LowCoinEnvironmentTests)
{
  Json::Value currencyRoot, itemRoot;
  std::ifstream currencyStream("lowCoinCurrencies.json");
  std::ifstream itemStream("testItems.json");
  CoinDispenserTests::TestLoad(currencyStream, itemStream, currencyRoot, itemRoot);
  ASSERT_EQ(currencyRoot.size(), 3);
  ASSERT_FLOAT_EQ(currencyRoot[0]["value"].asFloat(), .10f);
  ASSERT_FLOAT_EQ(currencyRoot[1]["value"].asFloat(), .25f);
  ASSERT_FLOAT_EQ(currencyRoot[2]["value"].asFloat(), .05f);
  ASSERT_EQ(currencyRoot[0]["numberOfCoins"].asUInt(), 6);
  ASSERT_EQ(currencyRoot[1]["numberOfCoins"].asUInt(), 0);
  ASSERT_EQ(currencyRoot[2]["numberOfCoins"].asUInt(), 1);
  std::vector<unsigned> coinsDispensed;
  coinsDispensed.resize(currencyRoot.size());
  bool exactChangeFlag = false;
  RegisterNormalCallback
    (DispenseCoin, 
    /*Captures DispenseCoin event*/
    [&coinsDispensed] (const Event* gEvent) 
      { 
        const DispenseCoin* iEvent = (const DispenseCoin*) gEvent;
        ++coinsDispensed[iEvent->dispenserId];
      }
    );
  RegisterNormalCallback
    (ExactChangeEvent, 
    /*Captures DispenseCoin event*/
    [&exactChangeFlag] (const Event* gEvent) 
      { 
        const ExactChangeEvent* iEvent = (const ExactChangeEvent*) gEvent;
        exactChangeFlag = iEvent->exactChangeOnly;
      }
    );
  
  CoinDispenser cDispenser(currencyRoot, itemRoot);
  EXPECT_TRUE(exactChangeFlag);
  EXPECT_FLOAT_EQ(cDispenser.GetTotalChangeValue(), .65f);
  EXPECT_EQ(cDispenser.GetMaxItemValue(), 1.00);
  CurrencyTaken currency25(.25f, TYPE_ID(CoinTemplate));
  CurrencyTaken currency10(.10f, TYPE_ID(CoinTemplate));
  CurrencyTaken currency05(.05f, TYPE_ID(CoinTemplate));
  /*Simulating a non-coin (such as a dollar)*/
  CurrencyTaken currency100(1.00f, TYPE_ID(CoinTemplate)+1);
  /*****Begin tests*****/ 
  /*Add currency but not coin*/
  SendEvent(currency100);
  EXPECT_TRUE(exactChangeFlag);
  /***/
  SendEvent(currency25); /*Dispensers: 6,1,1*/
  EXPECT_FLOAT_EQ(cDispenser.GetTotalChangeValue(), .90f);
  EXPECT_TRUE(exactChangeFlag);
  /***/
  SendEvent(currency10); /*Dispensers: 7,1,1*/
  EXPECT_FLOAT_EQ(cDispenser.GetTotalChangeValue(), 1.00f);
  EXPECT_TRUE(exactChangeFlag);
  /***/
  SendEvent(currency05); /*Dispensers: 7,1,2*/
  EXPECT_FLOAT_EQ(cDispenser.GetTotalChangeValue(), 1.05f);
  EXPECT_FALSE(exactChangeFlag);
  /***/
  DispenseChange dispenseChange(.80f);
  SendEvent(dispenseChange); /*Dispensers: 2,0,1*/
  EXPECT_EQ(coinsDispensed[0], 5);
  EXPECT_EQ(coinsDispensed[1], 1);
  EXPECT_EQ(coinsDispensed[2], 1);
  CoinDispenserTests::ResetArray(coinsDispensed);
  EXPECT_FLOAT_EQ(cDispenser.GetTotalChangeValue(), .25f);
  EXPECT_TRUE(exactChangeFlag);
  /**Not defined by a story, but I have decided that this system will react by doing what it can
     to pay back the user.**/
  dispenseChange = .50f;
  SendEvent(dispenseChange); /*Only returns .25... Dispensers: 0,0,0*/
  EXPECT_EQ(coinsDispensed[0], 2);
  EXPECT_EQ(coinsDispensed[1], 0);
  EXPECT_EQ(coinsDispensed[2], 1);
  EXPECT_FLOAT_EQ(cDispenser.GetTotalChangeValue(), 0.0f);
  EXPECT_TRUE(exactChangeFlag);
  CoinDispenserTests::ResetArray(coinsDispensed);
  /***/
  EventSystem::Teardown();
}
