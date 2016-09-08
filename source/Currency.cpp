/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Defines various currency types that are used by the CurrencyManager.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/

#include "Currency.hpp"
#include <assert.h>

CoinTemplate::CoinTemplate(const Json::Value& dataTemplate)
{
  assert(dataTemplate.isMember("weight"));
  assert(dataTemplate.isMember("weightErrorPercentage"));
  assert(dataTemplate.isMember("radius"));
  assert(dataTemplate.isMember("radiusErrorPercentage"));
  assert(dataTemplate.isMember("value"));
  /**/
  weight = dataTemplate["weight"].asFloat();
  weightErr = dataTemplate["weightErrorPercentage"].asFloat();
  radius = dataTemplate["radius"].asFloat();
  radiusErr = dataTemplate["radiusErrorPercentage"].asFloat();
  value = dataTemplate["value"].asFloat();
}

/*Bit of a violation of DRY, but it is only 2 values. If more, it would be good to abstract out the values and
  iterate through them.*/
bool CoinTemplate::TestValidity(const Json::Value& mysteryCurrency) const
{
  /*If it doesn't have these memebers, return false*/
  if(!mysteryCurrency.isMember("weight"))
    return false;
  if(!mysteryCurrency.isMember("radius"))
    return false;
  float testValue = mysteryCurrency["weight"].asFloat();
  if( (testValue < weight - weight*weightErr) || (testValue > weight + weight*weightErr) )
    return false;
  testValue = mysteryCurrency["radius"].asFloat();
  if( (testValue < radius - radius*weightErr) || (testValue > radius + radius*radiusErr) )
    return false;
  return true;
}

float CoinTemplate::GetCurrencyValue() const
{
  return value;
}
