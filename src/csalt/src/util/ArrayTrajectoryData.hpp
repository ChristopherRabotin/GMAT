//------------------------------------------------------------------------------
//                             ArrayTrajectoryData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Jeremy Knittel / NASA
// Created: 2017.02.23
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------
    
#ifndef ArrayTrajectoryData_hpp
#define ArrayTrajectoryData_hpp

#include "csaltdefs.hpp"
#include "TrajectoryData.hpp"
#include "OCHTrajectorySegment.hpp"

class CSALT_API ArrayTrajectoryData : public TrajectoryData
{
public:
   ArrayTrajectoryData();
   ArrayTrajectoryData(const ArrayTrajectoryData &copy);
   ArrayTrajectoryData& operator=(const ArrayTrajectoryData &copy);
   virtual ~ArrayTrajectoryData();
   
   virtual Real ProcessTimeValue(Real input, std::string timeSystem);
   virtual void ReadFromArray(Rmatrix guessArray, std::string coordSystem,
                              std::string timeSystem, std::string centralBody);
   virtual void WriteToFile(std::string fileName);
   
protected:

};

#endif
