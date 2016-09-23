/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Tests for the ConsoleManager.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
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
  EXPECT_EQ(items.size(), 3u);
}

TEST(ConsoleManagerTests, VerifiedCoinTests)
{
  Json::Value root;
  std::ifstream stream("testItems.json");
  std::vector<float> items;
  ConsoleManagerTests::TestLoad(stream, root, items);
  ConsoleManager mgr(root);
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
  Json::Value root;
  std::ifstream stream("testItems.json");
  std::vector<float> items;
  ConsoleManagerTests::TestLoad(stream, root, items);
  ConsoleManager mgr(root);
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
   EXPECT_EQ(itemIndex, 0u);
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
   EXPECT_EQ(itemIndex, 1u);
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
   EXPECT_EQ(itemIndex, 0u);
   EXPECT_FLOAT_EQ(changeDue, .25f);
   EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
   changeDue = 0.0f;
   EventSystem::Teardown();
}

TEST(ConsoleManagerTests, CoinReturnTests)
{
  Json::Value root;
  std::ifstream stream("testItems.json");
  std::vector<float> items;
  ConsoleManagerTests::TestLoad(stream, root, items);
  ConsoleManager mgr(root);
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

TEST(ConsoleManagerTests, SoldOutTests)
{
  Json::Value root;
  std::ifstream stream("lowQuantityTestItems.json");
  std::vector<float> items;
  ConsoleManagerTests::TestLoad(stream, root, items);
  ConsoleManager mgr(root);
  /*Quanities: Coke : 2, Chips: 1, Candy: 0*/
  bool itemDispensed;
  unsigned itemIndex;
  unsigned soldOutIndex;
  RegisterNormalCallback
    (DispenseItem, 
     /*Captures DispenseItem event.*/
     [&] (const Event* gEvent) 
      { 
        const DispenseItem* iEvent = (const DispenseItem*) gEvent;
        itemDispensed = true;
        itemIndex = iEvent->itemIndex;
      }
    );
  RegisterNormalCallback
    (SoldOutEvent, 
    /*Captures DispenseChange event*/
    [&] (const Event* gEvent) 
      { 
        const SoldOutEvent* iEvent = (const SoldOutEvent*) gEvent;
        itemDispensed = false;
        soldOutIndex = iEvent->indexSoldOut;
      }
    );
  CurrencyTaken addCurrency(1.00, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(2);
  /*Basic check*/
  SendEvent(addCurrency)
  SendEvent(itemChoice);
  EXPECT_FALSE(itemDispensed);
  EXPECT_EQ(soldOutIndex, 2u);
  /*Buy chips, then try to buy again*/
  itemChoice.index = 1;
  SendEvent(addCurrency);
  SendEvent(itemChoice);
  EXPECT_TRUE(itemDispensed);
  EXPECT_EQ(itemIndex, 1u);
  SendEvent(addCurrency);
  SendEvent(itemChoice);
  EXPECT_FALSE(itemDispensed);
  EXPECT_EQ(soldOutIndex, 1u);  
  /**Try to buy candy, then add money and buy soda.*/
  CurrencyTaken addCurrency2(.65f, TYPE_ID(CoinTemplate));
  CurrencyTaken addCurrency3(.35f, TYPE_ID(CoinTemplate));
  SendEvent(addCurrency2);
  itemChoice.index = 2;
  SendEvent(itemChoice);
  EXPECT_FALSE(itemDispensed);
  EXPECT_EQ(soldOutIndex, 2u);
  SendEvent(addCurrency3);
  itemChoice.index = 0;
  SendEvent(itemChoice);
  EXPECT_TRUE(itemDispensed);
  EXPECT_EQ(itemIndex, 0u);
  /**/
  EventSystem::Teardown();
}

TEST(ConsoleManagerTests, ExactChangeTests)
{
  Json::Value root;
  std::ifstream stream("testItems.json");
  std::vector<float> items;
  ConsoleManagerTests::TestLoad(stream, root, items);
  ConsoleManager mgr(root);
  /*Quanities: Coke : 2, Chips: 1, Candy: 0*/
  //bool itemDispensed = false;
  unsigned itemIndex = -1;
  float changeDue = -1.0f;
  RegisterNormalCallback
    (DispenseItem, 
     /*Captures DispenseItem event.*/
     [&] (const Event* gEvent) 
      { 
        const DispenseItem* iEvent = (const DispenseItem*) gEvent;
        itemIndex = iEvent->itemIndex;
      }
    );
  RegisterNormalCallback
    (DispenseChange, 
    /*Captures DispenseChange event*/
    [&] (const Event* gEvent) 
      { 
        const DispenseChange* iEvent = (const DispenseChange*) gEvent;
        changeDue = iEvent->changeAmount;
      }
    );
  CurrencyTaken addCurrency(1.00, TYPE_ID(CoinTemplate));
  CurrencyTaken addCurrency2(.50, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(1); /*.50*/
  ChooseChangeReturn changeReturn;
  ExactChangeEvent exactChangeEvent(true);
  /*Basic check - extra money*/
  SendEvent(exactChangeEvent);
  SendEvent(addCurrency)
  SendEvent(itemChoice);
  EXPECT_FLOAT_EQ(changeDue, -1.0f);
  EXPECT_EQ(itemIndex, 1u);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
  itemIndex = -1;
  changeDue = -1.0f;
  /*Basic check - exact change*/
  SendEvent(addCurrency2);
  SendEvent(itemChoice);
  EXPECT_FLOAT_EQ(changeDue, -1.0f);
  EXPECT_EQ(itemIndex, 1u);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
  itemIndex = -1;
  changeDue = -1.0f;
  /*Add money -> exact flag change -> dispense change*/
  SendEvent(addCurrency);
  exactChangeEvent.exactChangeOnly = false;
  SendEvent(exactChangeEvent);
  SendEvent(itemChoice);
  EXPECT_FLOAT_EQ(changeDue, .50f);
  EXPECT_EQ(itemIndex, 1u);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
  itemIndex = -1;
  changeDue = -1.0f;
  /*Check to see if returning coins still returns money.*/
  SendEvent(addCurrency);
  exactChangeEvent.exactChangeOnly = true;
  SendEvent(exactChangeEvent);
  SendEvent(changeReturn);
  EXPECT_FLOAT_EQ(changeDue, 1.00f);
  EXPECT_EQ(itemIndex, (unsigned)-1);
  EXPECT_FLOAT_EQ(mgr.GetCurrentAmountInserted(), 0.0f);
  /***/
  EventSystem::Teardown();
}
