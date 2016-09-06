#pragma once

#include "gtest/gtest.h"
#include "Event.hpp"
#include "EventSystem.hpp"
#include <string>
#include <sstream>
namespace EventSystemTests
{
  int callbackGlobal = 0;
  /*Making silly classes to make my life easier.*/
  class FooEvent : public Event
  {
  public:
    std::string data;
    FooEvent(const std::string& newStr) : Event(TYPE_ID(FooEvent)),  data(newStr) {}
    ~FooEvent() {}
  };

  class AppendEvent : public Event
  {
  public:
    std::string appData;
    AppendEvent(const std::string& newStr) : Event(TYPE_ID(AppendEvent)), appData(newStr) {}
    ~AppendEvent() {}
  };

  class BarEvent : public Event
  {
  public:
    BarEvent() : Event(TYPE_ID(BarEvent)) {}
    ~BarEvent() {}
  };

  struct SimpleClass
  {
    std::string testAnswer;
    int testInt;
    SimpleClass() : testInt(0) {}
    void SetupMessages()
    {
      RegisterClassCallback(FooEvent, *this, SimpleClass, OnFoo);
      RegisterClassCallback(BarEvent, *this, SimpleClass, OnBar);
    }
    void OnFoo(const Event* e)
    {
      testAnswer = ((FooEvent*)e)->data;
    }
    void OnBar(const Event* e)
    {
      ++testInt;
    }
  };

  void SimpleCallback(const Event* e) { EXPECT_NE(dynamic_cast<const FooEvent*>(e), nullptr); ++callbackGlobal; }
}

TEST(EventSystemTests, TestNormalCallbacks)
{
  /*Registrations*/
  RegisterNormalCallback(EventSystemTests::FooEvent, EventSystemTests::SimpleCallback);
  std::string testAnswer;
  RegisterNormalCallback
    (EventSystemTests::FooEvent, 
    /*Sets event data to testAnswer*/
    [&testAnswer] (const Event* gEvent) { const EventSystemTests::FooEvent* fEvent = (const EventSystemTests::FooEvent*) gEvent; testAnswer = fEvent->data; }
    );
  RegisterNormalCallback
    (EventSystemTests::AppendEvent, 
    /*Appends event data to testAnswer*/
    [&testAnswer] (const Event* gEvent) { const EventSystemTests::AppendEvent* fEvent = (const EventSystemTests::AppendEvent*) gEvent; testAnswer += fEvent->appData; }
    );
  /****/
  EventSystemTests::FooEvent testEvent("I am a test string");
  EXPECT_EQ(testAnswer, "");
  SendEvent(testEvent);  /*callbackGlobal == 1*/
  EXPECT_EQ(testAnswer, "I am a test string");
  /***/
  EventSystemTests::FooEvent testEvent2("I am a test string too");
  testAnswer.clear();
  EXPECT_EQ(testAnswer, "");
  SendEvent(testEvent2); /*callbackGlobal == 2*/
  EXPECT_EQ(testAnswer, "I am a test string too");
  /***/
  EventSystemTests::AppendEvent testEvent3(" adding words!");
  SendEvent(testEvent3);
  EXPECT_EQ(testAnswer, "I am a test string too adding words!");
  /***/
  SendEvent(testEvent); /*callbackGlobal == 3*/
  SendEvent(testEvent3);
  EXPECT_EQ(testAnswer, "I am a test string adding words!");
  /**/
  EXPECT_EQ(EventSystemTests::callbackGlobal, 3);
  EventSystem::Teardown();
}

TEST(EventSystemTests, TestOpposingNormalCallbacks)
{
  /*Registrations*/
  std::string testAnswer;
  std::string opposingAnswer;
  RegisterNormalCallback
    (EventSystemTests::FooEvent, 
    /*Sets event data to testAnswer*/
    [&testAnswer] (const Event* gEvent) { const EventSystemTests::FooEvent* fEvent = (const EventSystemTests::FooEvent*) gEvent; testAnswer = fEvent->data; }
    );
  RegisterNormalCallback
    (EventSystemTests::AppendEvent, 
    /*Appends event data to testAnswer*/
    [&testAnswer] (const Event* gEvent) { const EventSystemTests::AppendEvent* fEvent = (const EventSystemTests::AppendEvent*) gEvent; testAnswer += fEvent->appData; }
    );
    /**/
  RegisterNormalCallback
    (EventSystemTests::FooEvent, 
    /*Appends event data to opposingAnswer*/
    [&opposingAnswer] (const Event* gEvent) { const EventSystemTests::FooEvent* fEvent = (const EventSystemTests::FooEvent*) gEvent; opposingAnswer += fEvent->data; }
    );
  RegisterNormalCallback
    (EventSystemTests::AppendEvent, 
    /*Sets event data to opposingAnswer*/
    [&opposingAnswer] (const Event* gEvent) { const EventSystemTests::AppendEvent* fEvent = (const EventSystemTests::AppendEvent*) gEvent; opposingAnswer = fEvent->appData; }
    );
  /****/
  opposingAnswer = "I am test string opposition";
  EventSystemTests::FooEvent testEvent("I am a test string");
  SendEvent(testEvent);  
  EXPECT_EQ(testAnswer, "I am a test string");
  EXPECT_EQ(opposingAnswer, "I am test string oppositionI am a test string");
  /***/
  EventSystemTests::FooEvent testEvent2("I am a test string too");
  testAnswer.clear();
  SendEvent(testEvent2);
  EXPECT_EQ(testAnswer, "I am a test string too");
  EXPECT_EQ(opposingAnswer, "I am test string oppositionI am a test stringI am a test string too");
  /***/
  EventSystemTests::AppendEvent testEvent3(" adding words!");
  SendEvent(testEvent3);
  EXPECT_EQ(testAnswer, "I am a test string too adding words!");
  EXPECT_EQ(opposingAnswer, " adding words!");
  /***/
  SendEvent(testEvent);
  SendEvent(testEvent3);
  EXPECT_EQ(testAnswer, "I am a test string adding words!");
  EXPECT_EQ(opposingAnswer, " adding words!");
  /**/
  EventSystem::Teardown();
}

TEST(EventSystemTests, TestClassCallbacks)
{
  EventSystemTests::SimpleClass simpleClass;
  simpleClass.SetupMessages();
  EventSystemTests::FooEvent testEvent("I am a class test string");
  EXPECT_EQ(simpleClass.testAnswer, "");
  SendEvent(testEvent);
  EXPECT_EQ(simpleClass.testAnswer, "I am a class test string");
  /***/
  EventSystemTests::BarEvent testEvent2;
  EXPECT_EQ(simpleClass.testInt, 0);
  SendEvent(testEvent2);
  EXPECT_EQ(simpleClass.testInt, 1);
  /***/
  simpleClass.testInt = 112;
  SendEvent(testEvent2);
  EXPECT_EQ(simpleClass.testInt, 113);
  /***/
}
