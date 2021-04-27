//------------------------------------------------------------------------------
//                           TutorialDriver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Author: Noble Hatten
// Created: 2019/07/03
/**
 * The Tutorial test case driver
 */
 //------------------------------------------------------------------------------

#ifndef TutorialDriver_hpp
#define TutorialDriver_hpp

#include "CsaltTestDriver.hpp"

/**
 * Driver for the Tutorial problem
 */
class TutorialDriver : public CsaltTestDriver
{
public:
	TutorialDriver();
	virtual ~TutorialDriver();
	void StandAloneTest(); // method suitable for a standalone test like what is written in the User Guide Tutorial

protected:
	virtual void SetPointPathAndProperties();
	virtual void SetupPhases();
};

#endif /* TutorialDriver_hpp */
