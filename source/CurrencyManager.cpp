#include "CurrencyManager.hpp"
#include "Currency.hpp"
#include "CurrencyEvents.hpp"
#include "EventSystem.hpp"
#include <json\reader.h>
/*TODO: Put valid throw message*/
CurrencyManager::CurrencyManager(std::ifstream& currencyDefinitions)
{
  Json::Reader reader;
  Json::Value root;
  bool isGood = reader.parse(currencyDefinitions, root);
  if(isGood)
  {
    if(!root.isArray())
      throw;
    for(unsigned i = 0; i < root.size(); ++i)
    {
      if(!root[i].isObject() && root[i].isMember("type"))
        throw; /*Warnings would probally be better, but this is fine for now*/
      /*Could define meta meta files but it seems like overkill for now*/
      if(root[i]["type"].asString() == "Coin")
      {
        CurrencyTemplate* newCurrency = new CoinTemplate(root[i]);
        currencyTemplates.push_back(newCurrency);
      }
    }
  }
  else
    throw;
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
      CurrencyTaken currencyTaken(currencyTemplates[i]->GetCurrencyValue());
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