//$Id: IAUFile.cpp 9513 2012-02-24 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                            IAUFile.cpp
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
// Created: 2012/02/24
//
/**
 * Implements IAUFile class as specified in the GMAT Math Spec.
 */
//------------------------------------------------------------------------------

#include "IAUFile.hpp"
#include "MessageInterface.hpp"
#include <stdio.h>


//---------------------------------
// constant
//---------------------------------
const Integer IAUFile::MAX_TABLE_SIZE = 128;


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  IAUFile(const std::string &fileName = "IAU_SOFA.DAT", const Integer
//                 dim = 3);
//------------------------------------------------------------------------------
/**
 * Constructs IAUFile object (default constructor).
 * 
 * @param <fileName>  Name of IAU2000/2006 data file
 * @param <dim>       dimension of dependent vector
 */
//------------------------------------------------------------------------------
IAUFile::IAUFile(const std::string &fileName, Integer dim) :
   iauFileName		(fileName),
   independence		(NULL),
   dependences		(NULL),
   dimension		(dim),
   tableSz			(MAX_TABLE_SIZE),
   pointsCount		(0),
   isInitialized	(false)
{
}


//------------------------------------------------------------------------------
//  ~IAUFile()
//------------------------------------------------------------------------------
/**
 * Destroys IAUFile object (destructor).
 */
//------------------------------------------------------------------------------
IAUFile::~IAUFile()
{
   CleanupArrays();
}


//------------------------------------------------------------------------------
//  IAUFile(const IAUFile &iau)
//------------------------------------------------------------------------------
/**
 * Constructs IAUFile object, based on another (copy constructor).
 * 
 * @param iau The original that is being copied.
 */
//------------------------------------------------------------------------------
IAUFile::IAUFile(const IAUFile &iau) :
   iauFileName		(iau.iauFileName),
   dimension		(iau.dimension),
   tableSz			(iau.tableSz),
   pointsCount		(iau.pointsCount),
   isInitialized	(iau.isInitialized)
{
	// set values for independence and dependences arrays:
	if (iau.independence != NULL)
		CopyArrays(iau);
}


//------------------------------------------------------------------------------
//  IAUFile& operator=(const IAUFile &iau)
//------------------------------------------------------------------------------
/**
 * Sets this IAUFile object to match another (assignment operator).
 * 
 * @param iau The original that is being copied.
 * 
 * @return A reference to the copy (aka *this).
 */
//------------------------------------------------------------------------------
const IAUFile&
IAUFile::operator=(const IAUFile &iau)
{
   if (&iau == this)
      return *this;
   
   iauFileName		= iau.iauFileName;
   dimension		= iau.dimension;
   tableSz			= iau.tableSz;
   pointsCount		= iau.pointsCount;
   isInitialized	= iau.isInitialized;

   CopyArrays(iau);
      
   return *this;
}

void IAUFile::Initialize()
{
	// Allocate buffer to store IAU2000/2006 data:
	AllocateArrays();

	// Open IAU2000/2006 data file:
	FILE* fpt = fopen(iauFileName.c_str(), "r");

	// Read IAU2000/2006 data from data file and store to buffer: 
	Real t;
	Real XYs[3];
	int c;
	Integer i;
	for (i= 0; (c = fscanf(fpt, "%lf %lf %lf %lf\n",&t,&XYs[0],&XYs[1],&XYs[2])) != EOF; ++i)
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
			dependences[i][j] = XYs[j];

	}

	this->pointsCount = i;
}


#include "LagrangeInterpolator.hpp"
// get IAU2000 data for a given independence variable:
bool IAUFile::GetIAUData(Real ind, Real* iau_data, Integer dim, Integer order)
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
	Real stepsize = 1.0;
	Integer midpoint = (ind-independence[0])/stepsize;
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
	bool returnval = interpolator->Interpolate(ind, iau_data);
	delete interpolator;

	return returnval;
}



//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
//  void AllocateArrays()
//------------------------------------------------------------------------------
/**
 * Allocates IAUFile buffers to contain IAU2000/2006 data read from file.
 */
//------------------------------------------------------------------------------
void IAUFile::AllocateArrays()
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
void IAUFile::CleanupArrays()
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
//  void CopyArraysContain(const IAUFile &iau)
//------------------------------------------------------------------------------
/**
 * Copies the IAU2000/2006 buffer from one IAUFile object to this one.
 * 
 * @param iau The IAUFile object that supplies the data copied to this one.
 */
//------------------------------------------------------------------------------
void IAUFile::CopyArraysContent(const IAUFile &iau)
{
   // copy all contains of the array of independent variable:
   memcpy(independence, iau.independence, tableSz*sizeof(Real));

   // copy all contains of the array of dependent variables:
   memcpy(dependences, iau.dependences, tableSz*sizeof(Real*));

   Integer j;
   for (j = 0; j < tableSz; ++j)
      memcpy(dependences[j],  iau.dependences[j], dimension*sizeof(Real));
}


//------------------------------------------------------------------------------
//  void CopyArrays(const IAUFile &iau)
//------------------------------------------------------------------------------
/**
 * Copies the IAU2000/2006 buffer from one IAUFile object to this one.
 * 
 * @param iau The IAUFile object that supplies the data copied to this one.
 */
//------------------------------------------------------------------------------
void IAUFile::CopyArrays(const IAUFile &iau)
{
   // clean up buffer:
   CleanupArrays();

   // allocate buffer:
   AllocateArrays();

   // copy all contains:
   CopyArraysContent(iau);
}

