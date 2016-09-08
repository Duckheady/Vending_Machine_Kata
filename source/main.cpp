/*********************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  File Purpose: Runs the test suite and defines some globals. Only the "Deliverable" deployment is runnable "as-is" from the cmdline.
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***********************************************************************************/
#include "gtest/gtest.h"
#include "TypeId.hpp"
#include <iostream>
/***/
/*Defines the static function counter for types*/
static TYPEID type_id_counter = 0;
TYPEID GetNextUniqueId() { return ++type_id_counter; }
/****/
#include "EventSystemTests.hpp"
#include "CurrencyTests.hpp"
#include "ConsoleManagerTests.hpp"
#include "CoinDispenserTests.hpp"
#include "DisplayManagertests.hpp"
#include "VendingMachineIntegrationTests.hpp"

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  //std::cin.get();
  return 0;
}