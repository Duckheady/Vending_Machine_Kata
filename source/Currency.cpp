#include "Currency.hpp"

CoinTemplate::CoinTemplate(const Json::Value& dataTemplate)
{
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
