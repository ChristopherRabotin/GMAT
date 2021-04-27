//------------------------------------------------------------------------------
//                             OCH TrajectoryData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Jeremy Knittel / NASA
// Created: 2017.02.22
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------
    
#ifndef OCHTrajectoryData_hpp
#define OCHTrajectoryData_hpp

#include "csaltdefs.hpp"
#include "OCHTrajectorySegment.hpp"
#include "LowThrustException.hpp"
#include "StringUtil.hpp"
#include "ArrayTrajectoryData.hpp"
#include "TrajectoryData.hpp"
#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>

class CSALT_API OCHTrajectoryData : public TrajectoryData
{
public:
   /// default constructor
   OCHTrajectoryData(std::string fileName = "");
   /// copy constructor
   OCHTrajectoryData(const OCHTrajectoryData &copy);
   /// operator=
   OCHTrajectoryData& operator=(const OCHTrajectoryData &copy);
   
   /// destructor
   virtual ~OCHTrajectoryData();
   
  
   virtual void WriteToFile(std::string fileName);
   virtual void ReadFromFile(std::string fileName);

   virtual void SetNumSegments(Integer num);
   
   virtual void GetCentralBodyAndRefFrame(Integer forSegment,
                                          std::string &itsCentralBody,
                                          std::string &itsRefFrame);
   virtual void SetCentralBodyAndRefFrame(Integer forSegment,
                                          std::string newCentralBody,
                                          std::string newRefFrame);
   virtual void GetCentralBodyAndRefFrame(Real epoch,
                                          std::string &itsCentralBody,
                                          std::string &itsRefFrame);
   virtual void SetTimeSystem(Integer forSegment, std::string newTimeSys);

protected:

   virtual Real ProcessTimeString(std::string input, std::string timeSystem);

   /// Boolean determining whether or not the DATA section of the och file 
   /// was found
   bool dataFound;
   /// Boolean of whether or not extra data was detected in the och file that
   /// is ignored
   bool extraDataFound;
};

#endif
