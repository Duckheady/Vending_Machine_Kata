
#include "main.hpp"
#include "gtest/gtest.h"
#include "json/json.h"
#include "TypeId.hpp"
#include <string>
#include <iostream>
/***/
/*Defines the static function counter for types*/
static TYPEID type_id_counter = 0;
TYPEID GetNextUniqueId() { return ++type_id_counter; }
/****/
#include "EventSystemTests.hpp"
#include "CurrencyTests.hpp"
#include "ConsoleManagerTests.hpp"



int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  std::cin.get();
  return 0;
}