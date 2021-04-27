//$Id$
//------------------------------------------------------------------------------
//                         ClassName
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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

//#define DEBUG_CONSTRUCTION
//#define DEBUG_ACCESS


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
   Integer parmID;
   Integer covSize;

   //size_t pos = name.find_last_of('.');
   //if (pos != name.npos)
   //{
   //   std::string childObjFullName = owner->GetName() + "." + name.substr(0, pos);
   //   std::string paramName = name.substr(pos + 1);
   //   GmatBase* childObj = owner->GetRefObject(Gmat::UNKNOWN_OBJECT, childObjFullName);
   //   if (childObj == NULL)
   //      throw GmatBaseException("Error: Object with name '" + childObjFullName + "' was not defined in script.");

   //   parmID = childObj->GetParameterID(paramName);
   //   covSize = childObj->HasParameterCovariances(parmID);
   //}
   //else
   //{
      parmID = owner->GetParameterID(name);
      covSize = owner->HasParameterCovariances(parmID);
   //}

   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Adding covariance element %s with id %d "
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
            // if (elementOwners[i] == owner)
            // It needs to compare 2 objects by name in order to avoid cloned object
            if (elementOwners[i]->GetName() == owner->GetName())
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

         // It needs to set dimension for Rmatrix before using it
         if (theCovariance.GetNumRows() != 0)                                                                     // made changes by TUAN NGUYEN
         {                                                                                                        // made changes by TUAN NGUYEN
            Rmatrix temp = theCovariance;                                                                         // made changes by TUAN NGUYEN
            theCovariance.SetSize(dimension, dimension, true);                                                    // made changes by TUAN NGUYEN
            for (Integer row = 0; row < temp.GetNumRows(); ++row)                                                 // made changes by TUAN NGUYEN
            {                                                                                                     // made changes by TUAN NGUYEN
               for (Integer col = 0; col < temp.GetNumColumns(); ++col)                                           // made changes by TUAN NGUYEN
               {                                                                                                  // made changes by TUAN NGUYEN
                  theCovariance(row, col) = temp(row, col);                                                       // made changes by TUAN NGUYEN
               }                                                                                                  // made changes by TUAN NGUYEN
            }                                                                                                     // made changes by TUAN NGUYEN
         }                                                                                                        // made changes by TUAN NGUYEN
         else                                                                                                     // made changes by TUAN NGUYEN
            theCovariance.SetSize(dimension, dimension, true);
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


//---------------------------------------------------------------------------
// Integer GetDimension()
//---------------------------------------------------------------------------
/**
* Get dimension of covariance matrix
*
* @return     Dimension of covariance matrix
*/
//---------------------------------------------------------------------------
Integer Covariance::GetDimension()
{
   return dimension;
}


//---------------------------------------------------------------------------
// Rmatrix* GetCovariance()
//---------------------------------------------------------------------------
/**
* Get covariance matrix
*
* @return     Covariance matrix
*/
//---------------------------------------------------------------------------
Rmatrix *Covariance::GetCovariance()
{
   return &theCovariance;
}


// made changes by TUAN NGUYEN
//--------------------------------------------------------------------------
// Integer GetElementIndex(std::string elementName)
//--------------------------------------------------------------------------
/**
* This function is used to get the index associated to an element
*
* @param elementName    name of a given element 
*
* @return               index of the element when it found. 
*                       return -1 for case the element is not found.
*/
//--------------------------------------------------------------------------
Integer Covariance::GetElementIndex(std::string elementName)
{
   Integer index = -1;
   for (Integer i = 0; i < elementNames.size(); ++i)
   {
      if (elementNames[i] == elementName)
      {
         index = i;
      }
   }

   return index;
}


// made changes by TUAN NGUYEN
//--------------------------------------------------------------------------
// Integer GetElementID(std::string elementName)
//--------------------------------------------------------------------------
/**
* This function is used to get parameterID associated to an element
*
* @param elementName    name of a given element
*
* @return               parameterID of the element when it found.
*                       return -1 for case the element is not found.
*/
//--------------------------------------------------------------------------
Integer Covariance::GetElementID(std::string elementName)
{
   Integer parameterID = -1;
   for (Integer i = 0; i < elementNames.size(); ++i)
   {
      if (elementNames[i] == elementName)
      {
         parameterID = elementIndices[i];
      }
   }

   return parameterID;
}


// made changes by TUAN NGUYEN
//--------------------------------------------------------------------------
// Integer Covariance::GetElementSize(std::string elementName)
//--------------------------------------------------------------------------
/**
* This function is used to get size of covariance associated to the input
* element
*
* @param elementName    name of a given element
*
* @return               size of covariance associated to the element.
*                       return -1 if element not found
*/
//--------------------------------------------------------------------------
Integer Covariance::GetElementSize(std::string elementName)
{
   Integer index = GetElementIndex(elementName);
   Integer size = -1;
   if (index != -1)
      size = elementSizes[index];

   return size;
}


// made changes by TUAN NGUYEN
//--------------------------------------------------------------------------
// Integer Covariance::GetElementOwner(std::string elementName)
//--------------------------------------------------------------------------
/**
* This function is used to get owned object of the element
*
* @param elementName    name of a given element
*
* @return               the owned object of the element.
*                       return NULL if element not found
*/
//--------------------------------------------------------------------------
GmatBase* Covariance::GetElementOwner(std::string elementName)
{
   Integer index = GetElementIndex(elementName);
   GmatBase* owner = NULL;
   if (index != -1)
      owner = elementOwners[index];

   return owner;
}


//----------------------------------------------------------------------------
// Integer GetSubMatrixLocationStart(Integer forParameterID)
//----------------------------------------------------------------------------
/**
* Get the start index of a submatrix which is covariance of a given parameter.
*
* @param forParameterID    the ID of the parameter which needs to specified
*                          the start index of submatrix
*
* @return                  The start index of a submatrix to be covariance of 
*                          the given parameter
*/
//----------------------------------------------------------------------------
Integer Covariance::GetSubMatrixLocationStart(Integer forParameterID)
{
   Integer locationStart = 0;
   for (Integer i = 0; i < elementIndices.size(); ++i)
   {
      if (elementIndices[i] == forParameterID)
         break;
      locationStart += elementSizes[i];
   }

   return locationStart;
}


//----------------------------------------------------------------------------
// Integer GetSubMatrixLocationStart(const std::string paramName)
//----------------------------------------------------------------------------
/**
* Get the start index of a submatrix which is covariance of a given parameter.
*
* @param paramName    name of the parameter which needs to specified
*                     the start index of submatrix
*
* @return             The start index of a submatrix to be covariance of the 
*                     given parameter
*/
//----------------------------------------------------------------------------
Integer  Covariance::GetSubMatrixLocationStart(const std::string paramName)
{
   Integer index = 0;
   Integer locationStart = 0;
   for (; index < elementNames.size(); ++index)
   {
      if (elementNames[index] == paramName)
         break;
      locationStart += elementSizes[index];
   }
   if (index == elementNames.size())
      throw GmatBaseException("Error: cannot find covariance sub matrix for parameter " + paramName + ".\n");

   return locationStart;
}


//----------------------------------------------------------------------------
// Rmatrix* GetCovariance(Integer forParameterID)
//----------------------------------------------------------------------------
/**
* Get covariance matrix associated with a given parameter.
*
* @param forParameterID    the ID of the parameter which needs to specified
*                          covariance matrix
*
* @return                  the covariance matrix for the parameter
*/
//----------------------------------------------------------------------------
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


// made changes by TUAN NGUYEN
void Covariance::ShowContent()
{
   MessageInterface::ShowMessage("Covariance owner = <%p,%s>\n", covarianceOwner, (covarianceOwner==NULL?"":covarianceOwner->GetName().c_str()));
   MessageInterface::ShowMessage("Covariance dimesion = %d\n", dimension);
   MessageInterface::ShowMessage("Number of elements = %d\n", elementNames.size());
   for (Integer i = 0; i < elementNames.size(); ++i)
   {
      MessageInterface::ShowMessage("%d: ElementName = <%s>   ElementID = %d  ElementOwner = <%p,%s> ElementSize = %d\n", i, elementNames[i].c_str(), elementIndices[i], elementOwners[i], elementOwners[i]->GetName().c_str(), elementSizes[i]);
   }

   MessageInterface::ShowMessage("Covariance matrix = [\n");
   for (Integer row = 0; row < theCovariance.GetNumRows(); ++row)
   {
      for (Integer col = 0; col < theCovariance.GetNumColumns(); ++col)
      {
         MessageInterface::ShowMessage("%.15le   ", theCovariance(row,col));
      }
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("]\n");

}