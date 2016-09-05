
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

TEST(ThirdPartyLibrariesWork, Simple)
{
  Json::Value newValue;
  /*Testing to make sure third party libraries are working,
    and also give context of what I want.*/
  /***/
  newValue["weight"] = 2.99;
  newValue["radius"] = 3.22;
  /***/
  EXPECT_EQ(newValue["weight"], 2.99);
  EXPECT_EQ(newValue["radius"], 3.22);
  EXPECT_EQ(newValue["weightErrorPercentage"], .1f);
  EXPECT_EQ(newValue["radiusErrorPecentage"], .01f);
}





int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  std::cin.get();
  return 0;
}