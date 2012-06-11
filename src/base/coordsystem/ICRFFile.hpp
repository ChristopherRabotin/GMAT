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
   static ICRFFile* Instance();
   virtual void Initialize();	// initializes the ICRFFile (reads it and stores the data)
   void Finalize();

   // get ICRF Euler rotation vector for a given epoch:
   bool GetICRFRotationVector(Real epoch, Real* icrfRotationVector, Integer dim, Integer order);


private:
   // default constructor
   ICRFFile(const std::string &fileName = "ICRF_Table.txt", const Integer dim = 1);
   // copy constructor
//   ICRFFile(const ICRFFile &icrfFile);
   // operator = 
//   const ICRFFile& operator=(const ICRFFile &iau);
   // destructor
   virtual ~ICRFFile();
   
   static ICRFFile *instance;
   
protected:
//public:
   static const Integer MAX_TABLE_SIZE;

   std::string          icrfFileName;			// name of data file (It contains the path to the file)

   Real*				independence;			// a table containing values of independent variable 
   Real**				dependences;			// a table containing dependent vectors 
   Integer				dimension;				// dimension of dependent vector 
   Integer              tableSz;				// the size of the table
   Integer				pointsCount;			// number of data points

   bool isInitialized;							// specify whether the object is initialized or not
   
   void AllocateArrays();
   void CleanupArrays();
//   void CopyArrays(const ICRFFile &icrfFile);
//   void CopyArraysContent(const ICRFFile &icrfFile);
};


#endif // IAUFile_hpp
