/***************************************************************************************************
*  Author: Nicholas Louks
*  Kata: Vending Machine (C++)
*  Note: This code base is for  the Vending Machine code kata posed by Pillar Technologies.
*    This kata was developed for the purpose of displaying my ability to Pillar Technologies
*    in the hopes of being hired by Pillar. Please contact me with questions at nlouks@digipen.edu
***************************************************************************************************/

New material since last review:
 -Tests have been divided up to individually test each element as suggested. This has made the tests
  go up to 65 tests, but easier to manage in isolation.
 -New makefile for gnu. This file is found in the root directory. I recommend using make 
  [debug|release|deliverable]  make by itself generates all configurations. I didn't build a configure
  file as the differences were so small for this project. After running make, execute any executables
  from the /bin directory. (i.e. the working directory must be /bin) Debug and Release builds are found
  in /bin/<configuration> with the Visual Studio executables. Each executable from gnu is called 
  vending_machine_<configuration>.exe.
 -Only current block is the fail condition I asked about. I am unsure how to fail properly should fail
  conditions be met. My current suggestion is to have the display system display "OUT_OF_ORDER."
 -New Post-Mordem thoughts section.


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


Post-Mordem Thoughts:
  I am pleased by the comments thus far about my code. I see the flaw with tests and I have worked to 
  mitigate the issue. I have also worked in the GNU compiler as requested developing a makefile to 
  assist in development. I cannot remember if the standard GNU compiler comes with the binutils module,
  but you will need that if you don't have it as it is used to separate the symbols from the compiler in
  the development build. If not, the  debug and release build still have the symbols baked into the 
  executable so it should be sufficent for testing. Other than that, it was amusing working on this for 
  a bit longer. I am not an amazing build engineer, but I can build simple things as needed.