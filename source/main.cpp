
#include "main.hpp"
#include "gtest/gtest.h"
#include "json/json.h"
#include <string>
#include <objbase.h>
#include <iostream>
/***/

TEST(ThirdPartyLibrariesWork, Simple)
{
  Json::Value newValue;
  Json::Value materialValue;
  /*Testing to make sure third party libraries are working,
    and also give context of what I want.*/
  materialValue["copperPercentage"] = 75;
  materialValue["nickelPercentage"] = 25;
  /***/
  newValue["weight"] = 2.99;
  newValue["radius"] = 3.22;
  newValue["material"] = materialValue;
  newValue["weightErrorPercentage"] = .1f;
  newValue["radiusErrorPecentage"] = .01f;
  newValue["materialErrorPercentage"] = .01f;
  /***/
  EXPECT_EQ(newValue["weight"], 2.99);
  EXPECT_EQ(newValue["radius"], 3.22);
  EXPECT_EQ(newValue["material"]["copperPercentage"], 75);
  EXPECT_EQ(newValue["material"]["nickelPercentage"], 25);
  EXPECT_EQ(newValue["weightErrorPercentage"], .1f);
  EXPECT_EQ(newValue["radiusErrorPecentage"], .01f);
  EXPECT_EQ(newValue["materialErrorPercentage"], .01f);
}





int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  std::cin.get();
  return 0;
}