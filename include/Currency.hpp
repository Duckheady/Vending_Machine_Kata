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