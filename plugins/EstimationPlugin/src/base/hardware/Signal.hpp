//$Id: Sensor.hpp 67 2010-06-08 21:56:16Z tdnguye2@NDC $
//------------------------------------------------------------------------------
//                         Sensor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen
// Created: June 8, 2010
//
/**
 * The signal of a sensor.
 */
//------------------------------------------------------------------------------

#ifndef Signal_hpp
#define Signal_hpp

#include "estimation_defs.hpp"
#include "gmatdefs.hpp"

class ESTIMATION_API Signal
{
public:
	Signal();
	virtual ~Signal();
   Signal(const Signal& sig);
   Signal& operator=(const Signal& sig);


	void SetEpoch(GmatEpoch ep);
	GmatEpoch GetEpoch();
	Real GetValue();
	bool SetValue(Real v);

private:
	GmatEpoch epoch;
	Real value;
};

#endif /* Signal_hpp */
