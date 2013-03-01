//$Id: ICRFFile.cpp 9513 2012-02-24 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                            ICRFFile.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number #####
//
// Author: Tuan Nguyen (NASA/GSFC)
// Created: 2012/05/30
//
/**
 * Implements ICRFFile class as specified in the GMAT Math Spec.
 */
//------------------------------------------------------------------------------

#include "ICRFFile.hpp"
#include <stdio.h>
#include "LagrangeInterpolator.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Integer ICRFFile::MAX_TABLE_SIZE = 128;

ICRFFile*     ICRFFile::instance       = NULL;

//------------------------------------------------------------------------------
//  public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ICRFFile* Instance()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the instance of the singleton.
 *
 * @return pointer to the instance
 */
//------------------------------------------------------------------------------
ICRFFile* ICRFFile::Instance()
{
   if (instance == NULL)
	  instance = new ICRFFile("ICRF_Table.txt",3);	// this file contains a table of Euler rotation vectors for time range from 1957 to 2100

   return instance;
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the instance by reading data from the file.
 */
//------------------------------------------------------------------------------
void ICRFFile::Initialize()
{
   if (isInitialized)
      return;

   // Allocate buffer to store ICRF rotation vector table:
   AllocateArrays();

	// Open IAU2000/2006 data file:
   FileManager* thefile = FileManager::Instance();
   std::string path = thefile->GetPathname(FileManager::ICRF_FILE);
   std::string name = thefile->GetFilename(FileManager::ICRF_FILE);
   icrfFileName = path+name;
   FILE* fpt = fopen(icrfFileName.c_str(), "r");
   if (fpt == NULL)
	  throw GmatBaseException("Error: GMAT cann't open '" + icrfFileName + "' file!!!\n");

   // Read ICRF Euler rotation vector from data file and store to buffer:
   Real t;
   Real rotationvector[3];
   int c;
   Integer i;
   for (i= 0; (c = fscanf(fpt, "%lf, %le, %le, %le\n",&t,
	      &rotationvector[0],&rotationvector[1],&rotationvector[2])) != EOF; ++i)
   {
      // expend the buffer size when it has no room to contain data:
      if (i >= tableSz)
      {
         // create a new buffer with a larger size:
         Integer new_size = tableSz*2;
         Real* ind = new Real[new_size];
         Real** dep = new Real*[new_size];

         // copy contain in the current buffer to the new buffer:
         memcpy(ind, independence, tableSz*sizeof(Real));
         memcpy(dep, dependences, tableSz*sizeof(Real*));
         for (Integer k=tableSz; k < new_size; ++k)
            dep[k] = NULL;

         // delete the current buffer and use the new buffer as the current buffer:
         delete independence;
         delete dependences;
         independence = ind;
         dependences = dep;
         tableSz = new_size;
      }

      // store data to buffer:
      independence[i] = t;
      if (dependences[i] == NULL)
         dependences[i] = new Real[dimension];

      for (Integer j = 0; j < dimension; ++j)
         dependences[i][j] = rotationvector[j];
   }

   pointsCount = i;
   isInitialized = true;
}

//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/*
 * Finalizes the system by closing an opened file and deleting objects.
 */
//------------------------------------------------------------------------------
void ICRFFile::Finalize()
{
   CleanupArrays();
}

//------------------------------------------------------------------------------
// bool GetICRFRotationVector(Real ind, Real* icrfRotationVector, Integer dim,
//                            Integer order)
//------------------------------------------------------------------------------
/*
 * Get ICRF Euler rotation vector for a given epoch
 *
 * @param <ind>                  epoch at which Euler rotation vector needed
 * @param <icrfRotationVector>   the array containing the result of Euler
 *                               rotation vector
 * @param <dim>                  dimension of dependent vector
 * @param <order>                interpolation order
 */
//------------------------------------------------------------------------------
bool ICRFFile::GetICRFRotationVector(Real ind,    Real* icrfRotationVector,
                                     Integer dim, Integer order)
{
	// Verify the feasibility of interpolation:
	if ((independence == NULL)||(pointsCount == 0))
	{
		throw GmatBaseException("No data point is used for interpolation.\n");
	}
	else
	{
		if((ind < independence[0])||(ind > independence[pointsCount-1]))
		{
			throw GmatBaseException("The value of independent variable is out of range.\n");
		}

		if(order >= pointsCount)
		{
			throw GmatBaseException("Number of data points is not enough for interpolation.\n");
		}
	}

	// Specify beginning index and ending index in order to run interpolation:
	// The ICRF table has unequal step size. Therefore, we cannot use stepsize
	// to specify midpoint but binary search:
	Integer start = 0;
	Integer end = pointsCount-1;
	Integer midpoint;
	while (start < end-1)
	{
		midpoint = (start + end)/2;
		if (ind > independence[midpoint])
			start = midpoint;
		else
			end = midpoint;
	}

	Integer beginIndex = (0 > (midpoint-order/2))? 0:(midpoint-order/2);
	Integer endIndex = ((pointsCount-1) < (beginIndex+order))? (pointsCount-1):(beginIndex+order);
	beginIndex = (0 > (endIndex-order))? 0:(endIndex-order);

	// Run interpolation:
	// create an interpolator:
	LagrangeInterpolator* interpolator = new LagrangeInterpolator("", dim, order);

	// add data points in order to run interpolator:
	for (Integer i= beginIndex; i <= endIndex; ++i)
	{
		interpolator->AddPoint(independence[i], dependences[i]);
	}

	// run interpolator and get the result of dependent variables:
	interpolator->SetForceInterpolation(true);
	bool returnval = interpolator->Interpolate(ind, icrfRotationVector);
	delete interpolator;

	return returnval;
}


//------------------------------------------------------------------------------
//  protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void AllocateArrays()
//------------------------------------------------------------------------------
/**
 * Allocates ICRFFile buffers to contain ICRF Euler rotation vector read from file.
 */
//------------------------------------------------------------------------------
void ICRFFile::AllocateArrays()
{
   
   independence = new Real[tableSz];

   dependences = new Real*[tableSz];
   Integer i;
   for (i = 0; i < tableSz; ++i)
   {
      dependences[i]  = new Real[dimension];
   }
}


//------------------------------------------------------------------------------
//  void CleanupArrays()
//------------------------------------------------------------------------------
/**
 * Frees the memory used by the IAUFile buffer.
 */
//------------------------------------------------------------------------------
void ICRFFile::CleanupArrays()
{
   if (independence != NULL)
   {
	  // clean up the array of independent variable
	  delete independence;
	  independence = NULL;

	  // clean up the array of dependent variables
	  Integer i= 0;
	  for (i=0; i <tableSz; ++i)
	  {
		  if (dependences[i] != NULL)
			  delete dependences[i];
	  }

	  delete dependences;
	  dependences = NULL;
   }
}

//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  ICRFFile(const std::string &fileName = "ICRF_Table.txt",
//           const Integer dim = 3);
//------------------------------------------------------------------------------
/**
 * Constructs ICRFFile object (default constructor).
 *
 * @param <fileName>  Name of ICRF data file
 * @param <dim>       dimension of dependent vector
 */
//------------------------------------------------------------------------------
ICRFFile::ICRFFile(const std::string &fileName, Integer dim) :
   icrfFileName      (fileName),
   independence      (NULL),
   dependences       (NULL),
   dimension         (dim),
   tableSz           (MAX_TABLE_SIZE),
   pointsCount       (0),
   isInitialized     (false)
{
}

//------------------------------------------------------------------------------
//  ~ICRFFile()
//------------------------------------------------------------------------------
/**
 * Destroys ICRFFile object (destructor).
 */
//------------------------------------------------------------------------------
ICRFFile::~ICRFFile()
{
   CleanupArrays();
}

