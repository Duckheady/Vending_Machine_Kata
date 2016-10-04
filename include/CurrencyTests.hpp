/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Tests for the CurrencyManager.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
#pragma once

#include "gtest/gtest.h"
#include "Event.hpp"
#include "EventSystem.hpp"
#include <string>
#include <fstream>
#include <json\reader.h>
#include "Currency.hpp"
#include "CurrencyEvents.hpp"
#include "CurrencyManager.hpp"
#include "FailureExceptions.hpp"

/*I violate DRY here so failed tests show the developers exactly what is wrong*/
namespace CurrencyTests
{
  void TestLoad(std::ifstream& stream, Json::Value& root, std::vector<CoinTemplate>& coins)
  {
    ASSERT_TRUE(stream.good());
    Json::Reader streamReader;
    bool isGood = streamReader.parse(stream, root);
    ASSERT_TRUE(isGood);
    ASSERT_TRUE(root.isArray());
    for(unsigned i = 0; i < root.size(); ++i)
    {
      bool hasMember;
      hasMember = root[i].isMember("type");
      EXPECT_TRUE(hasMember);
      if(!hasMember)
        continue;
      EXPECT_EQ(root[i]["type"].asString(), "Coin");
      ASSERT_NO_THROW(coins.push_back(CoinTemplate(root[i])));
    }
  }

  bool TestAbstractCurrency(Json::Value abstractCurrency, const Json::Value& testValue, const std::string& testProperty, const CurrencyTemplate* currency, float noise)
  {
    float testFloat = testValue[testProperty].asFloat() + noise;
    abstractCurrency[testProperty] = testFloat;
    return currency->TestValidity(abstractCurrency);
  }

  void SendCurrencyInsertedEvent(Json::Value abstractCurrency, const Json::Value& testValue, const std::string& testProperty, float noise)
  {
    float testFloat = testValue[testProperty].asFloat() + noise;
    abstractCurrency[testProperty] = testFloat;
    CurrencyInserted cEvent(abstractCurrency);
    SendEvent(cEvent);
  }

  void RemoveAndTest(Json::Value& data, const std::string& memberName)
  {
    data[0].removeMember(memberName);
    try
    {
      CurrencyManager currencyManager(data);
      EventSystem::Teardown();
      EXPECT_FALSE(true);
    }
    catch(BadFileException) {}
  }
}

TEST(CurrencyLoadTest, TemplateCoinLoadingTest)
{
  Json::Value root;
  std::ifstream stream("testCurrencies.json");
  std::vector<CoinTemplate> coins;
  CurrencyTests::TestLoad(stream, root, coins);
  EXPECT_EQ(coins.size(), 3u);
}

TEST(CurrencyLoadTest, NoTemplateCoins)
{
  Json::Value nullValue;
  try
  {
    CurrencyManager currencyManager(nullValue);
    EventSystem::Teardown();
    EXPECT_FALSE(true);
  }
  catch(BadFileException) {}
}

class CurrencyTestFixture : public testing::Test
{
protected:
  Json::Value root;
  std::vector<CoinTemplate> coins;  
  virtual void SetUp()
  {
    std::ifstream stream("testCurrencies.json");
    CurrencyTests::TestLoad(stream, root, coins);
  }
public:
  virtual ~CurrencyTestFixture() {}
};

TEST_F(CurrencyTestFixture, BadCoinTest1)
{
  CurrencyTests::RemoveAndTest(root, "weight");
}
TEST_F(CurrencyTestFixture, BadCoinTest2)
{
  CurrencyTests::RemoveAndTest(root, "weightErrorPercentage");
}
TEST_F(CurrencyTestFixture, BadCoinTest3)
{
  CurrencyTests::RemoveAndTest(root, "radius");
}
TEST_F(CurrencyTestFixture, BadCoinTest4)
{
  CurrencyTests::RemoveAndTest(root, "radiusErrorPercentage");
}
TEST_F(CurrencyTestFixture, BadCoinTest5)
{
  CurrencyTests::RemoveAndTest(root, "value");
}
TEST_F(CurrencyTestFixture, CoinEquivilancyTestsNoVariance)
{
  Json::Value testCoin;
  /*Test both a bit over*/
  testCoin["radius"] = root[0]["radius"].asFloat();
  testCoin["weight"] = root[0]["weight"].asFloat();
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], 0));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[1], "radius", &coins[0], 0));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[2], "radius", &coins[0], 0));
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], 0));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[1], "weight", &coins[0], 0));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[2], "weight", &coins[0], 0));
}

TEST_F(CurrencyTestFixture, CoinEquivilancyTestsGoodNoiseTest)
{
  Json::Value testCoin;
  /*Test both a bit over*/
  testCoin["radius"] = root[0]["radius"].asFloat();
  testCoin["weight"] = root[0]["weight"].asFloat();
  float noise = .0001f;
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], noise));
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], -noise));
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], noise));
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], -noise));
}

TEST_F(CurrencyTestFixture, CoinEquivilancyTestsBadNoiseTest)
{
  Json::Value testCoin;
  /*Test both a bit over*/
  testCoin["radius"] = root[0]["radius"].asFloat();
  testCoin["weight"] = root[0]["weight"].asFloat();
  float noise = .1f;
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], noise));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], -noise));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], noise));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], -noise));
}

TEST_F(CurrencyTestFixture, CoinValueTests)
{
  for(unsigned i = 0; i < coins.size(); ++i)
  {
    EXPECT_EQ(coins[i].GetCurrencyValue(), root[i]["value"].asFloat());
  }
}

class CurrencyManagerTests : public CurrencyTestFixture
{
  void TestCurrency(unsigned testIndex, float noise, bool isBadNoise);
protected:
  CurrencyManager* currencyManager;
  bool tookCurrency;
  float valueOfCurrency;

  void CurrencyTakenCapture(const Event* gEvent)
  {
    const CurrencyTaken* cEvent = (const CurrencyTaken*) gEvent;
    tookCurrency = true;
    valueOfCurrency = cEvent->valueOfCurrency;
  }
  void CurrencyRejectedCapture(const Event*)
  {
    tookCurrency = false;
  }
  virtual void SetUp()
  {
    CurrencyTestFixture::SetUp();
    currencyManager = new CurrencyManager(root);
    tookCurrency = false;
    valueOfCurrency = 0;
    RegisterClassCallback(CurrencyTaken, *this, CurrencyManagerTests, CurrencyTakenCapture);
    RegisterClassCallback(CurrencyRejected, *this, CurrencyManagerTests, CurrencyRejectedCapture);
  }

  virtual void TearDown()
  {
    delete currencyManager;
    currencyManager = nullptr;
    EventSystem::Teardown();
    CurrencyTestFixture::TearDown();
  }
  
public:
  virtual ~CurrencyManagerTests() {}
};

TEST_F(CurrencyManagerTests, TestValidValuingOfCurrency0)
{
  Json::Value testCoin;
  /*Test both a bit over*/
  testCoin["radius"] = root[0]["radius"].asFloat();
  testCoin["weight"] = root[0]["weight"].asFloat();
  float testCoinValue = root[0]["value"].asFloat();
  float noise = .0001f;
  /*Should always invoke a taken event*/
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "radius", 0);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "radius", noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "radius", -noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "weight", 0);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "weight", noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "weight", -noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
}

TEST_F(CurrencyManagerTests, TestInvalidCurrency0)
{
  Json::Value testCoin;
  /*Test both a bit over*/
  testCoin["radius"] = root[0]["radius"].asFloat();
  testCoin["weight"] = root[0]["weight"].asFloat();
  float noise = .1f;
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "radius", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "radius", -noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "weight", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "weight", -noise);
  EXPECT_FALSE(tookCurrency);
}

TEST_F(CurrencyManagerTests, TestValidValuingOfCurrency2)
{
  Json::Value testCoin;
  /*Test both a bit over*/
  testCoin["radius"] = root[2]["radius"].asFloat();
  testCoin["weight"] = root[2]["weight"].asFloat();
  float testCoinValue = root[2]["value"].asFloat();
  float noise = .0001f;
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "radius", 0);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "radius", noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "radius", -noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "weight", 0);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "weight", noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "weight", -noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
}

TEST_F(CurrencyManagerTests, TestInvalidCurrency2)
{
  Json::Value testCoin;
  /*Test both a bit over*/
  testCoin["radius"] = root[2]["radius"].asFloat();
  testCoin["weight"] = root[2]["weight"].asFloat();
  float noise = .1f;
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "radius", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "radius", -noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "weight", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "weight", -noise);
  EXPECT_FALSE(tookCurrency);
}
