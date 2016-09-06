#pragma once
#include "Event.hpp"
#include <map>
#include <list>
#include <functional>

/*All functions to be called by the event system need the signature void (...)(const Event*)*/
/*See Event.hpp for the interface of event*/
typedef std::function<void(const Event*)> CallbackEvent;

/*Internal class used by Event System*/
class CallbackPackage
{
public:
  CallbackPackage() {}
  virtual ~CallbackPackage() {}
  virtual void InvokeCallback(const Event* event) = 0;
};

/*Simple event system so I can simulate inputs*/
/*Not going to worry about deregistering events, since I will, in this case, make sure
  everything is in scope that registers for the entirity of the event system's lifetime.
  Not ideal, but this is a coding test for vending machines not event systems.*/

class EventSystem
{
public:
  /*See TypeId.hpp*/
  DeclareSingleton(EventSystem)
  void RegisterCallbackPackage(TYPEID id, CallbackPackage* newPackage);
  void BroadcastEvent(const Event* event);
  ~EventSystem();
private:
  typedef std::map<TYPEID, std::list<CallbackPackage*> > CallbackStorage;
  CallbackStorage callbacks;
};

/****Callback implementations. These actually invoke the callbacks****/
class NormalCallback : public CallbackPackage
{
private:
  CallbackEvent callback;
public:
  NormalCallback(CallbackEvent cb) : callback(cb) {}
  virtual ~NormalCallback() {}
  virtual void InvokeCallback(const Event* event) { callback(event); }
};

template<class T>
class ClassCallbackPackage : public CallbackPackage
{
  typedef void (T::*ClassCallback)(const Event*);
  ClassCallback memberPtr;
  T* const classPtr;
public:
  ClassCallbackPackage(ClassCallback cb, T* ptr) : memberPtr(cb), classPtr(ptr) {}
  ~ClassCallbackPackage() {}
  void InvokeCallback(const Event* event) { (classPtr->*memberPtr)(event); }
};
/*******************/

/*This is what you need to know!!!*/
/*Defines a regular function / functor / lambda function for use in event system*/
#define RegisterNormalCallback(EventType, CallbackEvent) \
  { \
  CallbackPackage* newCallback = new NormalCallback(CallbackEvent); \
    EventSystem::GetInstance()->RegisterCallbackPackage(TYPE_ID(EventType), newCallback); \
  }

/*Defines a class function as a callback for use in the event system. Usualy use *this
  for the instance*/
#define RegisterClassCallback(EventType, ClassInstance, ClassType, MemberCallbackEvent) \
  { \
    CallbackPackage* newCallback = new ClassCallbackPackage<ClassType> (&ClassType::MemberCallbackEvent, &ClassInstance); \
    EventSystem::GetInstance()->RegisterCallbackPackage(TYPE_ID(EventType), newCallback); \
  }

/*Quicker way to say BroadcastEvent... interupts program flow*/
#define SendEvent(Event) \
  EventSystem::GetInstance()->BroadcastEvent(&Event);