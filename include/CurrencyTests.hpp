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
}

TEST(CurrencyTests, TemplateCoinLoadingTests)
{
  Json::Value root;
  std::ifstream stream("testCurrencies.json");
  std::vector<CoinTemplate> coins;
  CurrencyTests::TestLoad(stream, root, coins);
  EXPECT_EQ(coins.size(), 3);
}

TEST(CurrencyTests, CoinEquvilancyTests)
{
  Json::Value root;
  std::ifstream stream("testCurrencies.json");
  std::vector<CoinTemplate> coins;
  CurrencyTests::TestLoad(stream, root, coins);
  Json::Value testCoin;
  /*Test both a bit over*/
  testCoin["radius"] = root[0]["radius"].asFloat();
  testCoin["weight"] = root[0]["weight"].asFloat();
  float noise = .0001f;
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], 0));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[1], "radius", &coins[0], 0));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[2], "radius", &coins[0], 0));
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], 0));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[1], "weight", &coins[0], 0));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[2], "weight", &coins[0], 0));
  /**/
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], noise));
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], -noise));
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], noise));
  EXPECT_TRUE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], -noise));
  noise = .1f;
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], noise));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "radius", &coins[0], -noise));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], noise));
  EXPECT_FALSE(CurrencyTests::TestAbstractCurrency(testCoin, root[0], "weight", &coins[0], -noise));
}

TEST(CurrencyTests, CoinValueTests)
{
  Json::Value root;
  std::ifstream stream("testCurrencies.json");
  std::vector<CoinTemplate> coins;
  CurrencyTests::TestLoad(stream, root, coins);
  for(unsigned i = 0; i < coins.size(); ++i)
  {
    EXPECT_EQ(coins[i].GetCurrencyValue(), root[i]["value"].asFloat());
  }
}

TEST(CurrencyManagerTests, ValidEventsSent)
{
  Json::Value root;
  std::ifstream stream("testCurrencies.json");
  std::vector<CoinTemplate> coins;
  CurrencyTests::TestLoad(stream, root, coins);
  stream.clear();
  stream.seekg(0, std::ios::beg);
  CurrencyManager currencyManager(stream);
  currencyManager.RegisterEvents();
  bool tookCurrency = false;
  float valueOfCurrency = 0;
  RegisterNormalCallback
    (CurrencyTaken, 
    /*Captures CurrencyTaken event*/
    [&] (const Event* gEvent) 
      { 
        const CurrencyTaken* cEvent = (const CurrencyTaken*) gEvent;
        tookCurrency = true;
        valueOfCurrency = cEvent->valueOfCurrency;
      }
    );
  RegisterNormalCallback
    (CurrencyRejected, 
    /*Captures CurrencyRejected event*/
    [&tookCurrency] (const Event* gEvent) { tookCurrency = false; }
    );
  Json::Value testCoin;
  /*Test both a bit over*/
  testCoin["radius"] = root[0]["radius"].asFloat();
  testCoin["weight"] = root[0]["weight"].asFloat();
  float testCoinValue = root[0]["value"].asFloat();
  float noise = .0001f;
  /*Should always invoke a taken/rejected event*/
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
  /***/
  noise = .1f;
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "radius", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "radius", -noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "weight", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[0], "weight", -noise);
  EXPECT_FALSE(tookCurrency);
  /***Testing index 1 now***/
  testCoin["radius"] = root[1]["radius"].asFloat();
  testCoin["weight"] = root[1]["weight"].asFloat();
  testCoinValue = root[1]["value"].asFloat();
  noise = .0001f;
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "radius", 0);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "radius", noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "radius", -noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "weight", 0);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "weight", noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "weight", -noise);
  EXPECT_TRUE(tookCurrency);
  EXPECT_TRUE(valueOfCurrency == testCoinValue);
  /***/
  noise = .1f;
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "radius", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "radius", -noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "weight", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[1], "weight", -noise);
  EXPECT_FALSE(tookCurrency);
  /***Testing index 2 now***/
  testCoin["radius"] = root[2]["radius"].asFloat();
  testCoin["weight"] = root[2]["weight"].asFloat();
  testCoinValue = root[2]["value"].asFloat();
  noise = .0001f;
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
  /***/
  noise = .1f;
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "radius", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "radius", -noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "weight", noise);
  EXPECT_FALSE(tookCurrency);
  CurrencyTests::SendCurrencyInsertedEvent(testCoin, root[2], "weight", -noise);
  EXPECT_FALSE(tookCurrency);
}