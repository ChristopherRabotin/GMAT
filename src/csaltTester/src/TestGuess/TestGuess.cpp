//============================================================================
// Name        : TestGuess.cpp
// Author      : Jeremy Knittel
// Version     :
// Copyright   : 
// Description : 
//============================================================================

#include <iostream>
#include "TestGuessDriver.hpp"

using namespace std;

int main() {

	
    TestGuessDriver *driver = new TestGuessDriver();
    driver->Run();
    delete driver;

	std::cout << "Exiting TestGuess." << std::endl;
	return 0;
}
