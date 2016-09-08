/***************************************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***************************************************************************************************/

Summary:
 This project implements the six stories described at https://github.com/guyroyse/vending-machine-kata.
 The project is built in C++ utilizing the VS2010 compiler. This project utilizes GoogleTest and 
 Jsoncpp as free-to-use libraries to assist in development. The project implements four major modules: 
 CurrencyManager, ConsoleManager, CoinDispenser, and DisplayManager. I also implement a basic event 
 system to communicate between these major modules. Each module has it's own .hpp file which implements
 test scenarios in isolation as <moduleName>Tests.hpp in addition to integration tests which are in 
 thier own tests file. Also check out the repo's comments for more insight on my thoughts during the 
 process.
  
CurrencyManager:
  This system takes abstract objects (in a json form) and determines if it should accept the currency 
  or reject the currency. If currency is accpeted, it is assigned a value. Currency.hpp's TestValidity
  does most of the work for validity. This implementation should easily expand to paper currency with
  a little extra work.
  
ConsoleManager:
  This system's role is to validate events from the input pad(console). (i.e. ChangeReturn and Validating
  item purchase) This system leaves it to the CoinDispenser to make change and reabstract back to real
  coins. 
   
CoinDispenser:
  This system's role is to maintain the change in the coin dispensers. This system takes currency values
  and triggers the appropriate change dispensers to return change to the customer. This system also 
  signals when it does not have enough change, triggering the exact change state.
   
DisplayManager:
  This system's role is purely to maintain the display's text based on events occuring in the system. As
  per the specifications, the GetDisplay() returns the strings denoted in the user stories.

Notes:
 -Project executable will build to a /bin/Debug and /bin/Release folder on build. These folders are 
  ignored by git. Only the Deliverable build is captured by version control.
 -Added filters only to show my organization scheme. Usually I would leave it out of the main code base's
  version control.
 -Go ahead and read my comments on version control / revert. I frequently leave comments on my version 
  control to help me write monthly reports.
 -Basic coding style guide: 
   -Comments are in /**/ commented out code is in: //. 
   -dataMembers/localVariables are camel-case. Functions are capitalized.
   -Macros are all caps except when I want it to appear to be a global function i.e. SendEvent().
 -Check out VendingMachineIntegrationTests.hpp for a good place to start code evaluation as it has some 
  comments in there relating to expectations.
 -Many tests purposefully violate the DRY principle because I value my errors to tell me where the
  problem is. I hope Pillar has the same value.

Post-Mordem Thoughts:
  This was a pretty amusing test. I haven't used Google Test in a couple years, so it was intersting using
  the library again. The test cases came out pretty nice; however, I am still a bit concerned how readable
  the tests are to you developers, but I hope that it is legible enough. If you need clarification, I can
  do so, just send me an email. My big question to the customer if this project were to continue would be
  the expected behavior if paper currency were implemented and returning thier money still must be honored,
  since it is possible to request money back in coins that is more than the inserted paper currency.
  Additionally, I would like to add some randomized tests and crash tests, but I wanted to actually get
  this project to you in the timeframe I promised. I hope I didn't go overboard on this kata. I have a 
  tendancy of striving for perfection as long as I can meet deliverables in a timely manner.