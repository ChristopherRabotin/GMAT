//$Id: ICRFFile.hpp 9513 2012-02-24 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                                  ICRFFile.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Tuan Nguyen (NASA/GSFC)
// Created: 2012/05/30
//
/**
 * Definition of the ICRFFile class.  This is the code that provides
 * ICRF Euler rotation vector.
 *
 */
//------------------------------------------------------------------------------

#ifndef ICRFFile_hpp
#define ICRFFile_hpp

#include "gmatdefs.hpp"

class GMAT_API ICRFFile
{
public:
   /// Instance of the class
   static ICRFFile* Instance();
   /// initializes the ICRFFile (reads it and stores the data)
   virtual void Initialize();
   /// finalizes the ICRFFile
   void Finalize();

   // Get ICRF Euler rotation vector for a given epoch
   bool GetICRFRotationVector(Real epoch, Real* icrfRotationVector, Integer dim, Integer order);

protected:
   static const Integer MAX_TABLE_SIZE;

   /// name of data file (It contains the path to the file)
   std::string          icrfFileName;

   /// a table containing values of independent variable
   Real*                independence;
   /// a table containing dependent vectors
   Real**               dependences;
   /// dimension of dependent vector
   Integer              dimension;
   /// the size of the table
   Integer              tableSz;
   /// number of data points
   Integer              pointsCount;

   /// specify whether the object is initialized or not
   bool isInitialized;

   void AllocateArrays();
   void CleanupArrays();

private:
   // default constructor
   ICRFFile(const std::string &fileName = "ICRF_Table.txt", const Integer dim = 1);
   // copy constructor - NOT IMPLEMENTED
   ICRFFile(const ICRFFile &icrfFile);
   // operator = - NOT IMPLEMENTED
   const ICRFFile& operator=(const ICRFFile &icrfFile);
   // destructor
   virtual ~ICRFFile();
   
   static ICRFFile *instance;
};
#endif // ICRFFile_hpp
