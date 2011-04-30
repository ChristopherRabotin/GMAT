//$Id$
//------------------------------------------------------------------------------
//                         ClassName
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/ /
//
/**
 * File description here.
 */
//------------------------------------------------------------------------------


#include "Covariance.hpp"
#include "GmatBase.hpp"
#include "MessageInterface.hpp"

Covariance::Covariance(GmatBase *owner) :
   covarianceOwner   (owner),
   subCovariance     (NULL),
   dimension         (0)
{
}

Covariance::~Covariance()
{
   if (subCovariance != NULL)
      delete subCovariance;

}


Covariance::Covariance(const Covariance &cov) :
   covarianceOwner   (cov.covarianceOwner),
   subCovariance     (NULL),
   elementNames      (cov.elementNames),
   elementIndices    (cov.elementIndices),
   elementSizes      (cov.elementSizes),
   elementOwners     (cov.elementOwners),
   dimension         (cov.dimension)
{
   if (cov.theCovariance.IsSized())
      theCovariance     = cov.theCovariance;
}


Covariance& Covariance::operator=(const Covariance &cov)
{
   if (&cov != this)
   {
      covarianceOwner   = cov.covarianceOwner;
      if (cov.theCovariance.IsSized())
         theCovariance     = cov.theCovariance;
      elementNames      = cov.elementNames;
      elementIndices    = cov.elementIndices;
      elementSizes      = cov.elementSizes;
      elementOwners     = cov.elementOwners;

      dimension         = cov.dimension;
   }

   return *this;
}


Real& Covariance::operator()(const Integer r, const Integer c)
{
   if ((r < 0 ) || (r >= dimension))
      throw GmatBaseException("Covariance row index out of bounds");

   if ((c < 0 ) || (c >= dimension))
      throw GmatBaseException("Covariance column index out of bounds");

   return theCovariance(r,c);
}


Real Covariance::operator()(const Integer r, const Integer c) const
{
   if ((r < 0 ) || (r >= dimension))
      throw GmatBaseException("Covariance row index out of bounds");

   if ((c < 0 ) || (c >= dimension))
      throw GmatBaseException("Covariance column index out of bounds");

   return theCovariance(r,c);
}


void Covariance::AddCovarianceElement(const std::string &name,
      GmatBase* owner)
{
   Integer parmID = owner->GetParameterID(name);
   Integer covSize = owner->HasParameterCovariances(parmID);

   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Adding covariance element %s with id %d"
            "to object named %s\n", name.c_str(), owner->GetParameterID(name),
            owner->GetName().c_str());
   #endif

   if (covSize > 0)
   {
      // Check to see if element already exists for this object; if not, add it
      Integer index = -1;
      for (UnsignedInt i = 0; i < elementNames.size(); ++i)
      {
         if (name == elementNames[i])
         {
            if (elementOwners[i] == owner)
            {
               index = i;
               break;
            }
         }
      }

      if (index == -1)
      {
         elementNames.push_back(name);
         elementIndices.push_back(parmID);
         elementSizes.push_back(covSize);
         elementOwners.push_back(owner);
         dimension += covSize;
      }
   }
   else
      throw GmatBaseException("Covariance handling for " + name +
            " is not implemented");

   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Covariance dimension is now %d\n",
            dimension);
   #endif
}

bool Covariance::ConstructLHS(const std::string& lhs)
{
   bool retval = false;

   if (lhs != "")
   {

   }

   PrepareMatrix();

   return retval;
}


bool Covariance::ConstructRHS(const std::string& rhs)
{
   bool retval = false;

   // Parse the input string here

   return retval;
}

bool Covariance::ConstructRHS(Rvector diagonal, Integer start)
{
   bool retval = false;

   if (diagonal.IsSized() == false)
      throw GmatBaseException("Diagonal covariance vector is not properly "
            "initialized");

   Integer length = diagonal.GetSize();
   if (start + length > dimension)
      throw GmatBaseException("Diagonal covariance vector is will not fit in "
            "the allocated covariance matrix");

   length += start;
   for (Integer i = start; i < length; ++i)
      for (Integer j = start; j < length; ++j)
         if (i == j)
            theCovariance(i, j) = diagonal(i - start);
         else
            theCovariance(i, j) = 0.0;

   return retval;
}


bool Covariance::ConstructRHS(Rmatrix data, Integer start)
{
   bool retval = false;

   if (data.IsSized() == false)
      throw GmatBaseException("Input covariance matrix is not properly "
            "initialized");

   Integer length = data.GetNumRows();
   if (data.GetNumColumns() != length)
      throw GmatBaseException("Input covariance matrix is not square");

   if (start + length > dimension)
      throw GmatBaseException("Input covariance matrix is will not fit in "
            "the allocated covariance matrix");

   for (Integer i = 0; i < length; ++i)
      for (Integer j = i; j < length; ++j)
         if (i == j)
            theCovariance(i+start, j+start) = data(i, j);
         else
         {
            // Symmetrize as we go
            theCovariance(i+start, j+start) = theCovariance(j+start, i+start)
                                            = (data(i, j) + data(j, i)) / 2.0;
         }

   return retval;
}


bool Covariance::FillMatrix(const Rmatrix& rhs, bool overrideAndFill)
{
   bool retval = true;

   // Check sizes of the matrices
   if (!overrideAndFill && (dimension != rhs.GetNumRows()))
      throw GmatBaseException("Covariance assignment dimensions do not match");
   if (rhs.GetNumRows() != rhs.GetNumColumns())
      throw GmatBaseException("Input covariance matrix is not square");

   // Fill in the matrix info
   if (dimension != rhs.GetNumRows())
   {
      // Must be in override and fill mode; names, indices, sizes & owners are
      // all invalid; clear & set as unknown
      elementNames.clear();
      elementIndices.clear();
      elementSizes.clear();
      elementOwners.clear();

      elementNames.push_back("GenericCovariance");
      elementIndices.push_back(-1);
      // set dimension to the input size
      dimension = rhs.GetNumRows();
      elementSizes.push_back(dimension);
      elementOwners.push_back(NULL);
   }
   theCovariance = rhs;

   return retval;
}

void Covariance::SetDimension(Integer size)
{
   dimension = size;
   PrepareMatrix();
}


Integer Covariance::GetDimension()
{
   return dimension;
}


Rmatrix *Covariance::GetCovariance()
{
   return &theCovariance;
}


Rmatrix *Covariance::GetCovariance(Integer forParameterID)
{
   // Find the covariance elements that match up with the input ID
   Integer locationStart = 0;
   if (subCovariance != NULL)
   {
      delete subCovariance;
      subCovariance = NULL;
   }

   for (UnsignedInt i = 0; i < elementIndices.size(); ++i)
   {
      #ifdef DEBUG_ACCESS
         MessageInterface::ShowMessage("Checking if index %d == %d\n",
               elementIndices[i], forParameterID);
      #endif

      if (elementIndices[i] == forParameterID)
      {
         // Found it!
         Integer parmSize = elementSizes[i];
         subCovariance = new Rmatrix(parmSize, parmSize);

         for (Integer j = 0; j < parmSize; ++j)
         {
            for (Integer k = 0; k < parmSize; ++k)
            {
               (*subCovariance)(j, k) = theCovariance(locationStart+j,
                     locationStart+k);
            }
         }

         break;
      }
      locationStart += elementSizes[i];
   }

   return subCovariance;
}


void Covariance::PrepareMatrix()
{
   theCovariance.SetSize(dimension, dimension);

   // Initialize to the identity
   for (Integer i = 0; i < dimension; ++i)
      for (Integer j = 0; j < dimension; ++j)
         theCovariance(i, j) = (i == j ? 1.0 : 0.0);
}
