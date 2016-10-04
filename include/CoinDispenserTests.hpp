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
#include "FailureExceptions.hpp"

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

TEST(CoinDispenserTests, LoadingTest1)
{
  Json::Value currencyRoot, itemRoot;
  std::ifstream currencyStream("testCurrencies.json");
  std::ifstream itemStream("testItems.json");
  CoinDispenserTests::TestLoad(currencyStream, itemStream, currencyRoot, itemRoot);
  ASSERT_EQ(currencyRoot.size(), 3u);
  ASSERT_EQ(itemRoot.size(), 3u);
}

TEST(CoinDispenserTests, LoadingTest2)
{
  Json::Value currencyRoot, itemRoot;
  std::ifstream currencyStream("lowCoinCurrencies.json");
  std::ifstream itemStream("testItems.json");
  CoinDispenserTests::TestLoad(currencyStream, itemStream, currencyRoot, itemRoot);
  ASSERT_EQ(currencyRoot.size(), 3u);
  ASSERT_EQ(itemRoot.size(), 3u);
}

TEST(CoinDispenserTests, NoFileLoadingTest)
{
  Json::Value nullValue;
  try
  {
    CoinDispenser dispenser(nullValue, nullValue);
    EventSystem::Teardown();
    EXPECT_FALSE(true);
  }
  catch(BadFileException) {}
}

class CoinDispenserBadStreamTests : public testing::Test
{
protected:
  Json::Value currencyRoot, itemRoot;
  CoinDispenser* cDispenser;
  virtual void SetUp()
  {
    std::ifstream currencyStream("testCurrencies.json");
    std::ifstream itemStream("testItems.json");
    CoinDispenserTests::TestLoad(currencyStream, itemStream, currencyRoot, itemRoot);
  }

  void RemoveAndTestCurrency(const std::string& member)
  {
    currencyRoot[0].removeMember(member);
    try
    {
      CoinDispenser dispenser(currencyRoot, itemRoot);
      EventSystem::Teardown();
      EXPECT_FALSE(true);
    }
    catch(BadFileException) {}
  }
  void RemoveAndTestItem(const std::string& member)
  {
    itemRoot[0].removeMember(member);
    try
    {
      CoinDispenser dispenser(currencyRoot, itemRoot);
      EventSystem::Teardown();
      EXPECT_FALSE(true);
    }
    catch(BadFileException) {}
  }
};

TEST_F(CoinDispenserBadStreamTests, NoFileLoadingTest1)
{
  Json::Value nullValue;
  try
  {
    CoinDispenser dispenser(nullValue, itemRoot);
    EventSystem::Teardown();
    EXPECT_FALSE(true);
  }
  catch(BadFileException) {}
}

TEST_F(CoinDispenserBadStreamTests, NoFileLoadingTest2)
{
  Json::Value nullValue;
  try
  {
    CoinDispenser dispenser(currencyRoot, nullValue);
    EventSystem::Teardown();
    EXPECT_FALSE(true);
  }
  catch(BadFileException) {}
}
 
TEST_F(CoinDispenserBadStreamTests, BadCurrencyStreamTest1)
{
  RemoveAndTestCurrency("type");
}

TEST_F(CoinDispenserBadStreamTests, BadCurrencyStreamTest2)
{
  RemoveAndTestCurrency("value");
}

TEST_F(CoinDispenserBadStreamTests, BadCurrencyStreamTest3)
{
  RemoveAndTestCurrency("numberOfCoins");
}

TEST_F(CoinDispenserBadStreamTests, BadCurrencyStreamTest4)
{
  RemoveAndTestCurrency("dispenserId");
}

TEST_F(CoinDispenserBadStreamTests, BadItemTest)
{
  RemoveAndTestItem("cost");
}

class CoinDispenserSimpleDispenseTests : public testing::Test
{
protected:
  std::vector<unsigned> coinsDispensed;
  CoinDispenser* cDispenser;

  void DispenseCoinCapture(const Event* gEvent)
  {
    const DispenseCoin* iEvent = (const DispenseCoin*) gEvent;
    ++coinsDispensed[iEvent->dispenserId];
  }

  virtual void SetUp()
  {
    Json::Value currencyRoot, itemRoot;
    std::ifstream currencyStream("testCurrencies.json");
    std::ifstream itemStream("testItems.json");
    CoinDispenserTests::TestLoad(currencyStream, itemStream, currencyRoot, itemRoot);
    ASSERT_EQ(currencyRoot.size(), 3u);
    ASSERT_FLOAT_EQ(currencyRoot[0]["value"].asFloat(), .10f);
    ASSERT_FLOAT_EQ(currencyRoot[1]["value"].asFloat(), .25f);
    ASSERT_FLOAT_EQ(currencyRoot[2]["value"].asFloat(), .05f);
    coinsDispensed.resize(currencyRoot.size());
    CoinDispenserTests::ResetArray(coinsDispensed);
    cDispenser = new CoinDispenser(currencyRoot, itemRoot);
    RegisterClassCallback(DispenseCoin, *this, CoinDispenserSimpleDispenseTests, DispenseCoinCapture);
  }

  virtual void TearDown()
  {
    delete cDispenser;
    cDispenser = nullptr;
    EventSystem::Teardown();
  }
public:
  ~CoinDispenserSimpleDispenseTests() {}
};

TEST_F(CoinDispenserSimpleDispenseTests, SimpleTest1)
{
  DispenseChange dispenseChange(.50);
  /***/  
  SendEvent(dispenseChange);
  EXPECT_EQ(coinsDispensed[0], 0u); /*.10*/
  EXPECT_EQ(coinsDispensed[1], 2u); /*.25*/
  EXPECT_EQ(coinsDispensed[2], 0u); /*.05*/
}

TEST_F(CoinDispenserSimpleDispenseTests, SimpleTest2)
{
  DispenseChange dispenseChange(.65f);
  /***/  
  SendEvent(dispenseChange);
  EXPECT_EQ(coinsDispensed[0], 1u); /*.10*/
  EXPECT_EQ(coinsDispensed[1], 2u); /*.25*/
  EXPECT_EQ(coinsDispensed[2], 1u); /*.05*/
}

TEST_F(CoinDispenserSimpleDispenseTests, SimpleTest3)
{
  DispenseChange dispenseChange(.20f);
  /***/  
  SendEvent(dispenseChange);
  EXPECT_EQ(coinsDispensed[0], 2u); /*.10*/
  EXPECT_EQ(coinsDispensed[1], 0u); /*.25*/
  EXPECT_EQ(coinsDispensed[2], 0u); /*.05*/
}

TEST_F(CoinDispenserSimpleDispenseTests, SimpleTest4)
{
  DispenseChange dispenseChange(.05f);
  SendEvent(dispenseChange);
  EXPECT_EQ(coinsDispensed[0], 0u); /*.10*/
  EXPECT_EQ(coinsDispensed[1], 0u); /*.25*/
  EXPECT_EQ(coinsDispensed[2], 1u); /*.05*/
}

class CoinDispenserLowCoinTests : public testing::Test
{
protected:
  std::vector<unsigned> coinsDispensed;
  bool exactChangeFlag;
  CoinDispenser* cDispenser;

  void DispenseCoinCapture(const Event* gEvent)
  {
    const DispenseCoin* iEvent = (const DispenseCoin*) gEvent;
    ++coinsDispensed[iEvent->dispenserId];
  }
  void ExactChangeCapture(const Event* gEvent)
  {
    const ExactChangeEvent* iEvent = (const ExactChangeEvent*) gEvent;
    exactChangeFlag = iEvent->exactChangeOnly;
  }
  virtual void SetUp()
  {
    Json::Value currencyRoot, itemRoot;
    std::ifstream currencyStream("lowCoinCurrencies.json");
    std::ifstream itemStream("testItems.json");
    CoinDispenserTests::TestLoad(currencyStream, itemStream, currencyRoot, itemRoot);
    ASSERT_EQ(currencyRoot.size(), 3u);
    ASSERT_FLOAT_EQ(currencyRoot[0]["value"].asFloat(), .10f);
    ASSERT_FLOAT_EQ(currencyRoot[1]["value"].asFloat(), .25f);
    ASSERT_FLOAT_EQ(currencyRoot[2]["value"].asFloat(), .05f);
    ASSERT_EQ(currencyRoot[0]["numberOfCoins"].asUInt(), 6u);
    ASSERT_EQ(currencyRoot[1]["numberOfCoins"].asUInt(), 0u);
    ASSERT_EQ(currencyRoot[2]["numberOfCoins"].asUInt(), 1u);
    exactChangeFlag = false;
    coinsDispensed.resize(currencyRoot.size());
    CoinDispenserTests::ResetArray(coinsDispensed);
    RegisterClassCallback(DispenseCoin, *this, CoinDispenserLowCoinTests, DispenseCoinCapture);
    RegisterClassCallback(ExactChangeEvent, *this, CoinDispenserLowCoinTests, ExactChangeCapture);
    cDispenser = new CoinDispenser(currencyRoot, itemRoot);
    EXPECT_TRUE(exactChangeFlag);
    EXPECT_FLOAT_EQ(cDispenser->GetTotalChangeValue(), .65f);
    EXPECT_EQ(cDispenser->GetMaxItemValue(), 1.00);
  }

  virtual void TearDown()
  {
    delete cDispenser;
    cDispenser = nullptr;
    EventSystem::Teardown();
  }
public:
  ~CoinDispenserLowCoinTests() {}
};

/*THESE TESTS DEPEND ON THE INDIVIDUAL VALUES OF THE DATA!*/
/*dispenser0 == .10, dispenser1 == .25, dispenser2 == .05*/

TEST_F(CoinDispenserLowCoinTests, ExactChangeTest1)
{
  /*Simulating a non-coin (such as a dollar) just to keep exact change flag
    going*/
  CurrencyTaken currency100(1.00f, TYPE_ID(CoinTemplate)+1);
  /*Add currency but not coin*/
  SendEvent(currency100);
  EXPECT_TRUE(exactChangeFlag);  
}

TEST_F(CoinDispenserLowCoinTests, ExactChangeTest2)
{
  CurrencyTaken currency25(.25f, TYPE_ID(CoinTemplate));
  SendEvent(currency25); /*Dispensers: 6,1,1*/
  EXPECT_FLOAT_EQ(cDispenser->GetTotalChangeValue(), .90f);
  EXPECT_TRUE(exactChangeFlag);
}

TEST_F(CoinDispenserLowCoinTests, ExactChangeTest3)
{
  CurrencyTaken currency25(.25f, TYPE_ID(CoinTemplate));
  CurrencyTaken currency10(.10f, TYPE_ID(CoinTemplate));
  SendEvent(currency25); /*Dispensers: 6,1,1*/
  SendEvent(currency10); /*Dispensers: 7,1,1*/
  EXPECT_FLOAT_EQ(cDispenser->GetTotalChangeValue(), 1.00f);
  EXPECT_TRUE(exactChangeFlag);
}

TEST_F(CoinDispenserLowCoinTests, ExactChangeTest4)
{
  CurrencyTaken currency25(.25f, TYPE_ID(CoinTemplate));
  CurrencyTaken currency10(.10f, TYPE_ID(CoinTemplate));
  CurrencyTaken currency05(.05f, TYPE_ID(CoinTemplate));
  SendEvent(currency25); /*Dispensers: 6,1,1*/
  SendEvent(currency10); /*Dispensers: 7,1,1*/
  SendEvent(currency05); /*Dispensers: 7,1,2*/
  EXPECT_FLOAT_EQ(cDispenser->GetTotalChangeValue(), 1.05f);
  EXPECT_FALSE(exactChangeFlag);
}

TEST_F(CoinDispenserLowCoinTests, CoinReturnTest1)
{
  CurrencyTaken currency25(.25f, TYPE_ID(CoinTemplate));
  CurrencyTaken currency10(.10f, TYPE_ID(CoinTemplate));
  CurrencyTaken currency05(.05f, TYPE_ID(CoinTemplate));
  DispenseChange dispenseChange(.80f);
  SendEvent(currency25); /*Dispensers: 6,1,1*/
  SendEvent(currency10); /*Dispensers: 7,1,1*/
  SendEvent(currency05); /*Dispensers: 7,1,2*/
  EXPECT_FLOAT_EQ(cDispenser->GetTotalChangeValue(), 1.05f);
  SendEvent(dispenseChange); /*Dispensers: 2,0,1*/
  EXPECT_EQ(coinsDispensed[0], 5u);
  EXPECT_EQ(coinsDispensed[1], 1u);
  EXPECT_EQ(coinsDispensed[2], 1u);
  EXPECT_FLOAT_EQ(cDispenser->GetTotalChangeValue(), .25f);
  EXPECT_TRUE(exactChangeFlag);
}

TEST_F(CoinDispenserLowCoinTests, CoinReturnTest2)
{
  DispenseChange dispenseChange(.75f);
  SendEvent(dispenseChange); /*Only returns .65... Dispensers: 0,0,0*/
  EXPECT_EQ(coinsDispensed[0], 6u);
  EXPECT_EQ(coinsDispensed[1], 0u);
  EXPECT_EQ(coinsDispensed[2], 1u);
  EXPECT_FLOAT_EQ(cDispenser->GetTotalChangeValue(), 0.0f);
  EXPECT_TRUE(exactChangeFlag);
}
