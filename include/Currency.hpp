#pragma once
#include <json/value.h>
/*Mostly, just prediction for paper money and debit cards...*/
class CurrencyTemplate
{
public:
  virtual ~CurrencyTemplate() {}
  virtual bool TestValidity(const Json::Value& abstractObject) = 0;
  virtual float GetCurrencyValue() = 0;
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
  bool TestValidity(const Json::Value& mysteryCurrency);
  float GetCurrencyValue();
};

/*Addition of more currency types would go here*/