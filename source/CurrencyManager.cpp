#include "CurrencyManager.hpp"
#include "Currency.hpp"
#include "CurrencyEvents.hpp"
#include "EventSystem.hpp"
#include "FailureExceptions.hpp"
#include <json\reader.h>

CurrencyManager::CurrencyManager(const Json::Value& root)
{
  if(!root.isArray())
    throw BadFileException();
  for(unsigned i = 0; i < root.size(); ++i)
  {
    if(!root[i].isObject())
      throw BadFileException();
    if(!root[i].isMember("type"))
      throw BadFileException();
    /*Could define meta meta files but it seems like overkill for now*/
    if(root[i]["type"].asString() == "Coin")
    {
      CurrencyTemplate* newCurrency = new CoinTemplate(root[i]);
      currencyTemplates.push_back(newCurrency);
    }
  }
  RegisterEvents();
}

CurrencyManager::~CurrencyManager()
{
  for(unsigned i = 0; i < currencyTemplates.size(); ++i)
    delete currencyTemplates[i];
}

void CurrencyManager::OnCurrencyEntered(const Event* e)
{
  const CurrencyInserted* newCurrency = static_cast<const CurrencyInserted*>(e);
  /*foreach template, check if valid... if it is, take it's value and send an event out
    that valid currency entered, if not, send the event that it was rejected.*/
  for(unsigned i = 0; i < currencyTemplates.size(); ++i)
  {
    if(currencyTemplates[i]->TestValidity(newCurrency->coinData))
    {
      CurrencyTaken currencyTaken(currencyTemplates[i]->GetCurrencyValue(), currencyTemplates[i]->TypeId());
      SendEvent(currencyTaken);
      return;
    }
  }
  CurrencyRejected currencyRejected;
  SendEvent(currencyRejected);
}

void CurrencyManager::RegisterEvents()
{
  RegisterClassCallback(CurrencyInserted, *this, CurrencyManager, OnCurrencyEntered);
}