#pragma once

class Event;
class CoinDispenserManager
{
  void OnCurrencyTaken(void Event* e);
  void OnDispenseChange(void Event* e);
};