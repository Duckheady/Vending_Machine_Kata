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
#include "FailureExceptions.hpp"

namespace ConsoleManagerTests
{
  void TestLoad(std::ifstream& stream, Json::Value& root)
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
    }
  }

  void RemoveMemberAndTest(const std::string& member)
  {
    Json::Value badRoot;
    std::ifstream stream("testItems.json");
    ConsoleManagerTests::TestLoad(stream, badRoot);
    badRoot[0].removeMember(member);
    try
    {
      ConsoleManager badMgr(badRoot);
      EventSystem::Teardown();
      EXPECT_FALSE(true);
    }
    catch(BadFileException){}
  }

  /*Should never be called!*/
  void BadEvent(const Event*)
  {
    ASSERT_TRUE(false);
  }
}

TEST(ConsoleManagerTests, FileLoadingTest1)
{
  Json::Value root;
  std::ifstream stream("testItems.json");
  ConsoleManagerTests::TestLoad(stream, root);
}

TEST(ConsoleManagerTests, FileLoadingTest2)
{
  Json::Value root;
  std::ifstream stream("lowQuantityTestItems.json");
  ConsoleManagerTests::TestLoad(stream, root);
}

TEST(ConsoleManagerTests, NoFileTest)
{
  Json::Value nullValue;
  try
  {
    ConsoleManager badMgr(nullValue);
    EventSystem::Teardown();
    EXPECT_FALSE(true);
  }
  catch(BadFileException){}
}

TEST(ConsoleManagerTests, BadFileLoadingTest1)
{
  ConsoleManagerTests::RemoveMemberAndTest("cost");
}

TEST(ConsoleManagerTests, BadFileLoadingTest2)
{
  ConsoleManagerTests::RemoveMemberAndTest("quantity");
}

TEST(ConsoleManagerTests, VerifiedCoinTests)
{
  Json::Value root;
  std::ifstream stream("testItems.json");
  ConsoleManagerTests::TestLoad(stream, root);
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

class ConsoleManagerSelectItemTests : public testing::Test
{
protected:
  ConsoleManager* mgr;
  std::vector<float> items;
  unsigned itemIndex;
  bool notEnoughMoney;
  float priceOfItem;
  float changeDue;

  void DispenseItemCapture(const Event* gEvent)
  {
    const DispenseItem* iEvent = (const DispenseItem*) gEvent;
    itemIndex = iEvent->itemIndex;
    notEnoughMoney = false;
  }
  void DispenseChangeCapture(const Event* gEvent)
  {
    const DispenseChange* iEvent = (const DispenseChange*) gEvent;
    changeDue = iEvent->changeAmount;
  }
  void NotEnoughMoneyCapture(const Event* gEvent)
  {
    const NotEnoughMoneyEvent* iEvent = (const NotEnoughMoneyEvent*) gEvent;
    itemIndex = -1;
    notEnoughMoney = true;
    priceOfItem = iEvent->priceOfItem;
  }

  virtual void SetUp()
  {
    Json::Value root;
    std::ifstream stream("testItems.json");
    ConsoleManagerTests::TestLoad(stream, root);
    mgr = new ConsoleManager(root);
    //std::cout << "I am here" << std::endl;
    itemIndex = -1;
    //std::cout << "hey" << std::endl;
    changeDue = -1.0f;
    notEnoughMoney = false;
    priceOfItem = 0.0f;
    /*Captures DispenseItem event*/
    RegisterClassCallback(DispenseItem, *this, ConsoleManagerSelectItemTests, DispenseItemCapture)
    /*Captures DispenseChange event*/
    RegisterClassCallback(DispenseChange, *this, ConsoleManagerSelectItemTests, DispenseChangeCapture);
    /*Captures NotEnoughMoney event*/
    RegisterClassCallback(NotEnoughMoneyEvent, *this, ConsoleManagerSelectItemTests, NotEnoughMoneyCapture);
  }
  
  virtual void TearDown()
  {
    delete mgr;
    mgr = nullptr;
    EventSystem::Teardown();
  }
};

TEST_F(ConsoleManagerSelectItemTests, ValidSelectionTest)
{
  //std::cout << "In selection" << std::endl;
  CurrencyTaken addCurrency(1.50, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(0);
  SendEvent(addCurrency);
  SendEvent(itemChoice);
  //std::cout << "made to checks" << std::endl;
  EXPECT_FALSE(notEnoughMoney);
  EXPECT_EQ(itemIndex, 0u);
  EXPECT_FLOAT_EQ(changeDue, .50f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerSelectItemTests, InvalidToValidSelectionTest)
{
  CurrencyTaken addCurrency(.75, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(0);
  SendEvent(addCurrency);
  SendEvent(itemChoice);
  EXPECT_TRUE(notEnoughMoney);
  EXPECT_FLOAT_EQ(priceOfItem, 1.00f);
  EXPECT_FLOAT_EQ(changeDue, -1.0f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), .75f);
  itemChoice.index = 1;
  SendEvent(itemChoice);
  EXPECT_FALSE(notEnoughMoney);
  EXPECT_EQ(itemIndex, 1u);
  EXPECT_FLOAT_EQ(changeDue, .25f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerSelectItemTests, NoMoneySelection1)
{
  CurrencyTaken addCurrency(1.50, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(1);
  SendEvent(itemChoice);
  EXPECT_TRUE(notEnoughMoney);
  EXPECT_FLOAT_EQ(priceOfItem, .50f);
  EXPECT_FLOAT_EQ(changeDue, -1.0f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerSelectItemTests, NoMoneySelection2)
{
  CurrencyTaken addCurrency(1.50, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(2);
  SendEvent(itemChoice);
  EXPECT_TRUE(notEnoughMoney);
  EXPECT_FLOAT_EQ(priceOfItem, .65f);
  EXPECT_FLOAT_EQ(changeDue, -1.0f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerSelectItemTests, InsertInvalidInsertValidTest)
{
  /*Insert->Invalid->Insert->Valid*/
  CurrencyTaken addCurrency(.75, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(2);
  itemChoice.index = 0;
  SendEvent(addCurrency);
  SendEvent(itemChoice);
  EXPECT_TRUE(notEnoughMoney);
  EXPECT_FLOAT_EQ(priceOfItem, 1.00f);
  EXPECT_FLOAT_EQ(changeDue, -1.0f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), .75f);
  addCurrency.valueOfCurrency = .50f;
  SendEvent(addCurrency);
  SendEvent(itemChoice);
  EXPECT_FALSE(notEnoughMoney);
  EXPECT_EQ(itemIndex, 0u);
  EXPECT_FLOAT_EQ(changeDue, .25f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

class ConsoleManagerCoinReturnTests : public testing::Test
{
protected:
  ConsoleManager* mgr;
  float changeDue;

  void DispenseChangeCapture(const Event* gEvent)
  {
    const DispenseChange* iEvent = (const DispenseChange*) gEvent;
    changeDue = iEvent->changeAmount;
  }
  virtual void SetUp()
  {
    Json::Value root;
    std::ifstream stream("testItems.json");
    ConsoleManagerTests::TestLoad(stream, root);
    mgr = new ConsoleManager(root);
    changeDue = 0.0f;
    /*Captures DispenseItem event. Should never be called!*/
    RegisterNormalCallback(DispenseItem, ConsoleManagerTests::BadEvent);
    /*Captures DispenseChange event*/
    RegisterClassCallback(DispenseChange, *this, ConsoleManagerCoinReturnTests, DispenseChangeCapture);
  }
  
  virtual void TearDown()
  {
    delete mgr;
    mgr = nullptr;
    EventSystem::Teardown();
  }
};

TEST_F(ConsoleManagerCoinReturnTests, SimpleReturns1)
{
  CurrencyTaken addCurrency(1.50, TYPE_ID(CoinTemplate));
  ChooseChangeReturn changeReturn;
  SendEvent(addCurrency);
  SendEvent(changeReturn);
  EXPECT_FLOAT_EQ(changeDue, 1.50f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerCoinReturnTests, SimpleReturns2)
{
  CurrencyTaken addCurrency(.75, TYPE_ID(CoinTemplate));
  ChooseChangeReturn changeReturn;
  SendEvent(addCurrency);
  SendEvent(changeReturn);
  EXPECT_FLOAT_EQ(changeDue, .75f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerCoinReturnTests, SimpleReturns3)
{
  CurrencyTaken addCurrency(.75, TYPE_ID(CoinTemplate));
  ChooseChangeReturn changeReturn;
  /*Add twice, then returns*/
  SendEvent(addCurrency);
  SendEvent(addCurrency);
  SendEvent(changeReturn);
  EXPECT_FLOAT_EQ(changeDue, 1.50f);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerCoinReturnTests, NoMoneyReturn)
{
  ChooseChangeReturn changeReturn;
  /*No events should be sent if there is no change to be dispensed*/
  /*Should not be called!!!*/
  RegisterNormalCallback
    (DispenseChange, 
     /*Captures DispenseChange event. Should never be called!!!*/
     ConsoleManagerTests::BadEvent
    );
   /*The following should have no additonal events called!*/
   SendEvent(changeReturn);
}

class ConsoleManagerSoldOutTests : public testing::Test
{
  void SoldOutCapture(const Event* gEvent)
  { 
    const SoldOutEvent* iEvent = (const SoldOutEvent*) gEvent;
    itemDispensed = false;
    soldOutIndex = iEvent->indexSoldOut;
  }

  void DispenseItemCapture(const Event* gEvent)
  { 
    const DispenseItem* iEvent = (const DispenseItem*) gEvent;
    itemDispensed = true;
    itemIndex = iEvent->itemIndex;
  }

protected:
  ConsoleManager* mgr;
  bool itemDispensed;
  unsigned itemIndex;
  unsigned soldOutIndex;
  virtual void SetUp()
  {
    Json::Value root;
    std::ifstream stream("lowQuantityTestItems.json");
    ConsoleManagerTests::TestLoad(stream, root);
    mgr = new ConsoleManager(root);
    /*Quanities: Coke : 2, Chips: 1, Candy: 0*/
    itemDispensed = true;
    itemIndex = -1;
    soldOutIndex = -1;
    /*Captures DispenseItem event*/
    RegisterClassCallback(DispenseItem, *this, ConsoleManagerSoldOutTests, DispenseItemCapture)
    /*Captures DispenseChange event*/
    RegisterClassCallback(SoldOutEvent, *this, ConsoleManagerSoldOutTests, SoldOutCapture);
  }
  
  virtual void TearDown()
  {
    delete mgr;
    mgr = nullptr;
    EventSystem::Teardown();
  }
};

TEST_F(ConsoleManagerSoldOutTests, SimpleTest)
{
  /*Quanities: Coke : 2, Chips: 1, Candy: 0*/
  CurrencyTaken addCurrency(1.00, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(2);
  /*Basic check*/
  SendEvent(addCurrency)
  SendEvent(itemChoice);
  EXPECT_FALSE(itemDispensed);
  EXPECT_EQ(soldOutIndex, 2u);
}

TEST_F(ConsoleManagerSoldOutTests, BuyThenBuyTest1)
{
  /*Quanities: Coke : 2, Chips: 1, Candy: 0*/
  CurrencyTaken addCurrency(1.00, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(1);
  /*Buy chips, then try to buy again - second should fail*/
  SendEvent(addCurrency);
  SendEvent(itemChoice);
  EXPECT_TRUE(itemDispensed);
  EXPECT_EQ(itemIndex, 1u);
  SendEvent(addCurrency);
  SendEvent(itemChoice);
  EXPECT_FALSE(itemDispensed);
  EXPECT_EQ(soldOutIndex, 1u); 
}

TEST_F(ConsoleManagerSoldOutTests, BuyThenBuyTest2)
{
  /*Quanities: Coke : 2, Chips: 1, Candy: 0*/
  CurrencyTaken addCurrency(1.00, TYPE_ID(CoinTemplate));
  CurrencyTaken addCurrency2(.65f, TYPE_ID(CoinTemplate));
  CurrencyTaken addCurrency3(.35f, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(2);
  /**Try to buy candy, then add money and buy soda.*/
  SendEvent(addCurrency2);
  SendEvent(itemChoice);
  EXPECT_FALSE(itemDispensed);
  EXPECT_EQ(soldOutIndex, 2u);
  SendEvent(addCurrency3);
  itemChoice.index = 0;
  SendEvent(itemChoice);
  EXPECT_TRUE(itemDispensed);
  EXPECT_EQ(itemIndex, 0u);
}

class ConsoleManagerExactChangeTests : public testing::Test
{
protected:
  ConsoleManager* mgr;
  unsigned itemIndex;
  float changeDue;

  void DispenseItemCapture(const Event* gEvent)
  {
    const DispenseItem* iEvent = (const DispenseItem*) gEvent;
    itemIndex = iEvent->itemIndex;
  }
  void DispenseChangeCapture(const Event* gEvent)
  {
    const DispenseChange* iEvent = (const DispenseChange*) gEvent;
    changeDue = iEvent->changeAmount;
  }

  virtual void SetUp()
  {
    Json::Value root;
    std::ifstream stream("testItems.json");
    ConsoleManagerTests::TestLoad(stream, root);
    mgr = new ConsoleManager(root);
    itemIndex = -1;
    changeDue = -1.0f;
    RegisterClassCallback(DispenseItem, *this, ConsoleManagerExactChangeTests, DispenseItemCapture);
    RegisterClassCallback(DispenseChange, *this, ConsoleManagerExactChangeTests, DispenseChangeCapture);
  }
  
  virtual void TearDown()
  {
    delete mgr;
    mgr = nullptr;
    EventSystem::Teardown();
  }
};

TEST_F(ConsoleManagerExactChangeTests, ExtraMoneyTest)
{
  CurrencyTaken addCurrency(1.00, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(1); /*.50*/
  ExactChangeEvent exactChangeEvent(true);
  /*Basic check - extra money*/
  SendEvent(exactChangeEvent);
  SendEvent(addCurrency)
  SendEvent(itemChoice);
  EXPECT_FLOAT_EQ(changeDue, -1.0f);
  EXPECT_EQ(itemIndex, 1u);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerExactChangeTests, ExactChangeTest)
{
  CurrencyTaken addCurrency(1.00, TYPE_ID(CoinTemplate));
  CurrencyTaken addCurrency2(.50, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(1); /*.50*/
  ExactChangeEvent exactChangeEvent(true);
  /*Basic check - exact change*/
  SendEvent(exactChangeEvent);
  SendEvent(addCurrency2);
  SendEvent(itemChoice);
  EXPECT_FLOAT_EQ(changeDue, -1.0f);
  EXPECT_EQ(itemIndex, 1u);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerExactChangeTests, AddMoneyToOverThreshold)
{
  CurrencyTaken addCurrency(1.00, TYPE_ID(CoinTemplate));
  ChooseItemEvent itemChoice(1); /*.50*/
  ExactChangeEvent exactChangeEvent(true);
  /*Add money -> exact flag change -> dispense change*/
  SendEvent(exactChangeEvent);
  SendEvent(addCurrency);
  exactChangeEvent.exactChangeOnly = false;
  SendEvent(exactChangeEvent);
  SendEvent(itemChoice);
  EXPECT_FLOAT_EQ(changeDue, .50f);
  EXPECT_EQ(itemIndex, 1u);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}

TEST_F(ConsoleManagerExactChangeTests, ChangeReturnDuringExactChange)
{
  CurrencyTaken addCurrency(1.00, TYPE_ID(CoinTemplate));
  ChooseChangeReturn changeReturn;
  ExactChangeEvent exactChangeEvent(true);
  /*Check to see if returning coins still returns money.*/
  SendEvent(exactChangeEvent);
  SendEvent(addCurrency);  
  SendEvent(changeReturn);
  EXPECT_FLOAT_EQ(changeDue, 1.00f);
  EXPECT_EQ(itemIndex, (unsigned)-1);
  EXPECT_FLOAT_EQ(mgr->GetCurrentAmountInserted(), 0.0f);
}
