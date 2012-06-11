//$Id: IAUFile.hpp 9513 2012-02-24 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                                  IAUFile.hpp
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
// Created: 2012/02/24
//
/**
 * Definition of the IAUFile class.  This is the code that provides
 * IAU2000-2006 data
 *
 */
//------------------------------------------------------------------------------

#ifndef IAUFile_hpp
#define IAUFile_hpp

#include "gmatdefs.hpp"

class GMAT_API IAUFile
{
public:
   static IAUFile* Instance();
   virtual void Initialize();	// initializes the IAUFile (reads it and stores the data)
   void Finalize();

   // get IAU2000 data for a given epoch:
   bool GetIAUData(Real epoch, Real* iau, Integer dim, Integer order);

private:
   // default constructor
   IAUFile(const std::string &fileName = "IAU_SOFA.DAT", const Integer dim = 1);

   // destructor
   virtual ~IAUFile();
   
   static IAUFile *instance;
   
protected:
//public:
   static const Integer MAX_TABLE_SIZE;

   std::string          iauFileName;			// name of data file (It contains the path to the file) 

   Real*				independence;			// a table containing values of independent variable 
   Real**				dependences;			// a table containing dependent vectors 
   Integer				dimension;				// dimension of dependent vector 
   Integer              tableSz;				// the size of the table
   Integer				pointsCount;			// number of data points

   bool isInitialized;							// specify whether the object is initailized or not
   
   void AllocateArrays();
   void CleanupArrays();
//   void CopyArrays(const IAUFile &iau);
//   void CopyArraysContent(const IAUFile &iau);
};


#endif // ICRFFile_hpp
