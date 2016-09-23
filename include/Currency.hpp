/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Defines various currency types that are used by the CurrencyManager.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
#pragma once
#include "TypeId.hpp"
#include <json/value.h>
/*Mostly, just prediction for paper money and debit cards...*/
class CurrencyTemplate
{
public:
  virtual ~CurrencyTemplate() {}
  virtual bool TestValidity(const Json::Value& abstractObject) const = 0;
  virtual float GetCurrencyValue() const = 0;
  /*Inheritor Note: Just return TYPE_ID(<Type>)*/
  virtual TYPEID TypeId() const = 0;
};

/*Defining what a coin means, ask client what exactly should
  be the data we are checking for.*/
class CoinTemplate : public CurrencyTemplate
{
  float weight;
  float weightErr;
  float radius;
  float radiusErr;
  /**/
  float value;
public:
  CoinTemplate(const Json::Value& dataTemplate);
  ~CoinTemplate() {}
  bool TestValidity(const Json::Value& mysteryCurrency) const;
  float GetCurrencyValue() const;
  TYPEID TypeId() const { return TYPE_ID(CoinTemplate); }
};

/*Addition of more currency types would go here*/
