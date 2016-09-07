#pragma once

#include "gtest/gtest.h"
#include "EventSystem.hpp"
#include <fstream>
#include <json\reader.h>
#include "ConsoleManager.hpp"
#include "ConsoleEvents.hpp"
#include "CurrencyEvents.hpp"
#include "DispenserEvents.hpp"

namespace ConsoleManagerTests
{
  void TestLoad(std::ifstream& stream, Json::Value& root, std::vector<float>& items)
  {
    ASSERT_TRUE(stream.good());
    Json::Reader streamReader;
    bool isGood = streamReader.parse(stream, root);
    ASSERT_TRUE(isGood);
    ASSERT_TRUE(root.isArray());
    for(unsigned i = 0; i < root.size(); ++i)
    {
      bool hasMember;
      hasMember = root[i].isMember("cost");
      EXPECT_TRUE(hasMember);
      if(!hasMember)
        continue;
      items.push_back(root[i]["cost"].asFloat());
    }
  }

  /*Should never be called!*/
  void BadEvent(const Event*)
  {
    ASSERT_TRUE(false);
  }
}

TEST(ConsoleManagerTests, TemplateConsoleTests)
{
  Json::Value root;
  std::ifstream stream("testItems.json");
  std::vector<float> items;
  ConsoleManagerTests::TestLoad(stream, root, items);
  EXPECT_EQ(items.size(), 3);
}

TEST(ConsoleManagerTests, VerifiedCoinTests)
{
  std::ifstream stream("testItems.json");
  std::vector<float> items;
  ConsoleManager mgr(stream);
  CurrencyTaken addCurrency(.05f, TYPE_ID(CoinTemplate));
  CurrencyRejected rejectCur;
  SendEvent(addCurrency);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), .05f);
  addCurrency.valueOfCurrency = .10f;
  SendEvent(addCurrency);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), .15f);
  SendEvent(rejectCur);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), .15f);
  EventSystem::Teardown();
}

TEST(ConsoleManagerTests, SelectItemTests)
{
  std::ifstream stream("testItems.json");
  std::vector<float> items;
  ConsoleManager mgr(stream);
  unsigned itemIndex = -1;
  float changeDue = 0.0f;
  bool notEnoughMoney = false;
  float priceOfItem = 0.0f;
  RegisterNormalCallback
    (DispenseItem, 
    /*Captures DispenseItem event*/
    [&] (const Event* gEvent) 
      { 
        const DispenseItem* iEvent = (const DispenseItem*) gEvent;
        itemIndex = iEvent->itemIndex;
        notEnoughMoney = false;
      }
    );
   RegisterNormalCallback
    (DispenseChange, 
    /*Captures DispenseChange event*/
    [&changeDue] (const Event* gEvent) 
      { 
        const DispenseChange* iEvent = (const DispenseChange*) gEvent;
        changeDue = iEvent->changeAmount;
      }
    );
    RegisterNormalCallback
      (NotEnoughMoneyEvent, 
      /*Captures NotEnoughMoney event*/
      [&] (const Event* gEvent) 
      { 
        const NotEnoughMoneyEvent* iEvent = (const NotEnoughMoneyEvent*) gEvent;
        itemIndex = -1;
        notEnoughMoney = true;
        priceOfItem = iEvent->priceOfItem;
      }
    );
   /*Valid selection test*/
   CurrencyTaken addCurrency(1.50, TYPE_ID(CoinTemplate));
   ChooseItemEvent itemChoice(0);
   SendEvent(addCurrency);
   SendEvent(itemChoice);
   EXPECT_FALSE(notEnoughMoney);
   EXPECT_EQ(itemIndex, 0);
   EXPECT_FLOAT_EQ(changeDue, .50f);
   EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
   changeDue = 0.0f;
   /**************/
   /*Invalid into valid selection*/
   addCurrency.valueOfCurrency = .75;
   itemChoice.index = 0;
   SendEvent(addCurrency);
   SendEvent(itemChoice);
   EXPECT_TRUE(notEnoughMoney);
   EXPECT_FLOAT_EQ(priceOfItem, 1.00f);
   EXPECT_FLOAT_EQ(changeDue, 0.0f);
   EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), .75f);
   itemChoice.index = 1;
   SendEvent(itemChoice);
   EXPECT_FALSE(notEnoughMoney);
   EXPECT_EQ(itemIndex, 1);
   EXPECT_FLOAT_EQ(changeDue, .25f);
   EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
   changeDue = 0.0f;
   /**************/
   /*No money selections*/
   SendEvent(itemChoice);
   EXPECT_TRUE(notEnoughMoney);
   EXPECT_FLOAT_EQ(priceOfItem, .50f);
   EXPECT_FLOAT_EQ(changeDue, 0.0f);
   EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
   itemChoice.index = 2;
   SendEvent(itemChoice);
   EXPECT_TRUE(notEnoughMoney);
   EXPECT_FLOAT_EQ(priceOfItem, .65f);
   EXPECT_FLOAT_EQ(changeDue, 0.0f);
   EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
   /**************/
   /*Insert->Invalid->Insert->Valid*/
   addCurrency.valueOfCurrency = .75;
   itemChoice.index = 0;
   SendEvent(addCurrency);
   SendEvent(itemChoice);
   EXPECT_TRUE(notEnoughMoney);
   EXPECT_FLOAT_EQ(priceOfItem, 1.00f);
   EXPECT_FLOAT_EQ(changeDue, 0.0f);
   EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), .75f);
   addCurrency.valueOfCurrency = .50f;
   SendEvent(addCurrency);
   SendEvent(itemChoice);
   EXPECT_FALSE(notEnoughMoney);
   EXPECT_EQ(itemIndex, 0);
   EXPECT_FLOAT_EQ(changeDue, .25f);
   EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
   changeDue = 0.0f;
   EventSystem::Teardown();
}

TEST(ConsoleManagerTests, CoinReturnTests)
{
  std::ifstream stream("testItems.json");
  std::vector<float> items;
  ConsoleManager mgr(stream);
  float changeDue;
  RegisterNormalCallback
    (DispenseItem, 
     /*Captures DispenseItem event. Should never be called!*/
     ConsoleManagerTests::BadEvent
    );
  RegisterNormalCallback
    (DispenseChange, 
    /*Captures DispenseChange event*/
    [&changeDue] (const Event* gEvent) 
      { 
        const DispenseChange* iEvent = (const DispenseChange*) gEvent;
        changeDue = iEvent->changeAmount;
      }
    );
  /*Simple adds, then returns*/
  CurrencyTaken addCurrency(1.50, TYPE_ID(CoinTemplate));
  ChooseChangeReturn changeReturn;
  SendEvent(addCurrency);
  SendEvent(changeReturn);
  EXPECT_FLOAT_EQ(changeDue, 1.50f);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
  /***/
  addCurrency.valueOfCurrency = .75;
  SendEvent(addCurrency);
  SendEvent(changeReturn);
  EXPECT_FLOAT_EQ(changeDue, .75f);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
  /*Add twice, then returns*/
  SendEvent(addCurrency);
  SendEvent(addCurrency);
  SendEvent(changeReturn);
  EXPECT_FLOAT_EQ(changeDue, 1.50f);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
  /*No events should be sent if there is no change to be dispensed*/
  /*Should not be called!!!*/
  RegisterNormalCallback
    (DispenseChange, 
     /*Captures DispenseChange event. Should never be called!!!*/
     ConsoleManagerTests::BadEvent
    );
   /*The following should have no additonal events called!*/
   SendEvent(changeReturn);
   EventSystem::Teardown();
}
