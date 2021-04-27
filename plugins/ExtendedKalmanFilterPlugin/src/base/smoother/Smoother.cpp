//$Id$
//------------------------------------------------------------------------------
//                         Smoother
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/12
//
/**
 * Class for smoother
 */
//------------------------------------------------------------------------------

#include "Smoother.hpp"

#include "GmatConstants.hpp"
#include <sstream>
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"
#include "Solver.hpp"
#include "ODEModel.hpp"

// EKF mod 12/16
#include "GroundstationInterface.hpp"
#include "Spacecraft.hpp" 
#include "SpaceObject.hpp"
#include "StringUtil.hpp"
#include "Rmatrix66.hpp"
#include "StateConversionUtil.hpp"

#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>


//------------------------------------------------------------------------------
// Smoother(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The subtype of the derived class
 * @param name The name of the instance
 */
//------------------------------------------------------------------------------
Smoother::Smoother(const std::string &name) :
   SmootherBase            ("Smoother", name)
{
   objectTypeNames.push_back("Smoother");
}


//------------------------------------------------------------------------------
// ~Smoother()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Smoother::~Smoother()
{
}


//------------------------------------------------------------------------------
// Smoother::Smoother(const Smoother & sm) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param sm The smoother that provides configuration information for this one
 */
//------------------------------------------------------------------------------
Smoother::Smoother(const Smoother & sm) :
   SmootherBase            (sm)
{
}


//------------------------------------------------------------------------------
// Smoother& operator=(const Smoother &sm)
//------------------------------------------------------------------------------
/**
 * Assignemtn operator
 *
 * @param sm The smoother that provides configuration information for this one
 *
 * @return This smoother, configured to match sm
 */
//------------------------------------------------------------------------------
Smoother& Smoother::operator=(
      const Smoother &sm)
{
   if (this != &sm)
   {
      SmootherBase::operator=(sm);
   }
   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Object cloner
 *
 * @return Pointer to a new Smoother configured to match this one.
 */
 //------------------------------------------------------------------------------
GmatBase* Smoother::Clone() const
{
   return new Smoother(*this);
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
 //---------------------------------------------------------------------------
void  Smoother::Copy(const GmatBase* orig)
{
   operator=(*((Smoother*)(orig)));
}


//------------------------------------------------------------------------------
// void SmoothState(SmootherInfoType &smootherStat, bool includeUpdate)
//------------------------------------------------------------------------------
/**
 * Smooths the state based on the forward and backward filter data
 */
 //------------------------------------------------------------------------------
void Smoother::SmoothState(SmootherInfoType &smootherStat, bool includeUpdate)
{
   UnsignedInt backFilterIndex = FindIndex(forwardFilterInfo[filterIndex],
                                           backwardFilterInfo);

   Rvector forwardState = forwardFilterInfo[filterIndex].state;
   Rvector backwardState = backwardFilterInfo[backFilterIndex].state;

   Rmatrix forwardCov = forwardFilterInfo[filterIndex].cov;
   Rmatrix backwardCov = backwardFilterInfo[backFilterIndex].cov;

   smootherStat.epoch = forwardFilterInfo[filterIndex].epoch;
   smootherStat.isObs = forwardFilterInfo[filterIndex].isObs;

   if (smootherStat.isObs && includeUpdate)
   {
      Rvector forwardAprioriState = forwardFilterInfo[filterIndex].measStat.state;
      Rvector backwardAprioriState = backwardFilterInfo[backFilterIndex].measStat.state;

      Rmatrix forwardAprioriCov = forwardFilterInfo[filterIndex].measStat.cov;
      Rmatrix backwardAprioriCov = backwardFilterInfo[backFilterIndex].measStat.cov;

      Rmatrix weight1, weight2;

      try
      {
         weight1 = forwardCov * (forwardCov + backwardAprioriCov).Inverse(COV_INV_TOL);
         weight2 = forwardAprioriCov * (forwardAprioriCov + backwardCov).Inverse(COV_INV_TOL);
      }
      catch (Rmatrix::IsSingular ex)
      {
         throw EstimatorException("Smoother attempted to invert a singular or "
            "ill-conditioned covariance matrix when calculating the weighting matrix");
      }

      smootherStat.state = (forwardState + weight1 * (backwardAprioriState - forwardState)).GetRealArray();
      smootherStat.measStat.state = (forwardAprioriState + weight2 * (backwardState - forwardAprioriState)).GetRealArray();

      Rmatrix cov1 = forwardCov - weight1 * forwardCov;
      Symmetrize(cov1);
      smootherStat.cov.SetSize(cov1.GetNumRows(), cov1.GetNumColumns());
      smootherStat.cov = cov1;

      Rmatrix cov2 = forwardAprioriCov - weight2 * forwardAprioriCov;
      Symmetrize(cov2);
      smootherStat.measStat.cov.SetSize(cov2.GetNumRows(), cov2.GetNumColumns());
      smootherStat.measStat.cov = cov2;

      smootherStat.measStat.sigmaVNB = GetCovarianceVNB(smootherStat.measStat.cov);
   }
   else
   {
      if (smootherStat.isObs)
      {
         // Replace forward state & cov with pre-update values
         forwardState = forwardFilterInfo[filterIndex].measStat.state;
         forwardCov = forwardFilterInfo[filterIndex].measStat.cov;
      }

      Rmatrix weight;

      try
      {
         weight = forwardCov * (forwardCov + backwardCov).Inverse(COV_INV_TOL);
      }
      catch (Rmatrix::IsSingular ex)
      {
         throw EstimatorException("Smoother attempted to invert a singular or "
            "ill-conditioned covariance matrix when calculating the weighting matrix");
      }

      smootherStat.state = (forwardState + weight * (backwardState - forwardState)).GetRealArray();

      Rmatrix cov = forwardCov - weight * forwardCov;
      Symmetrize(cov);
      smootherStat.cov.SetSize(cov.GetNumRows(), cov.GetNumColumns());
      smootherStat.cov = cov;
   }

   smootherStat.sigmaVNB = GetCovarianceVNB(smootherStat.cov);
}


//------------------------------------------------------------------------------
// UnsignedInt FindIndex(SeqEstimator::UpdateInfoType &filterInfo,
//                       std::vector<SeqEstimator::UpdateInfoType> &filterInfoVector)
//------------------------------------------------------------------------------
/**
 * Find the index of a filter info struct that matches the provied struct
 */
 //------------------------------------------------------------------------------
UnsignedInt Smoother::FindIndex(SeqEstimator::UpdateInfoType &filterInfo,
                                std::vector<SeqEstimator::UpdateInfoType> &filterInfoVector)
{
   bool found = false;
   UnsignedInt searchIndex = 0;

   for (UnsignedInt ii = 0U; ii < filterInfoVector.size(); ii++)
   {
      if (GmatMathUtil::IsEqual(filterInfo.epoch, filterInfoVector[ii].epoch, ESTTIME_ROUNDOFF))
      {
         found = true;
         searchIndex = ii;

         if (ObsMatch(filterInfo, filterInfoVector[ii]))
            break;
      }
   }

   if (!found)
      throw EstimatorException("Unable to find a matching epoch between the forward and backward filter data while smoothing");

   return searchIndex;
}


//------------------------------------------------------------------------------
// bool ObsMatch(SeqEstimator::UpdateInfoType &filterInfo1,
//               SeqEstimator::UpdateInfoType &filterInfo2)
//------------------------------------------------------------------------------
/**
 * Find if the two filter info struct correspond to the same measurement(s)
 */
 //------------------------------------------------------------------------------
bool Smoother::ObsMatch(SeqEstimator::UpdateInfoType &filterInfo1,
                               SeqEstimator::UpdateInfoType &filterInfo2)
{
   // See if one has a measurement while the other doesn't
   if (filterInfo1.isObs != filterInfo2.isObs)
      return false;

   // If both are not measurements, we're done
   if (!filterInfo1.isObs)
      return true;

   // If both are measurements, need to compare record numbers
   // TODO: When batch update is implemented, will need to check that each element
   //       of recNum match
   if (filterInfo1.measStat.recNum == filterInfo2.measStat.recNum)
      return true;

   // Otherwise, false
   return false;
}


//------------------------------------------------------------------------------
// bool WriteAdditionalMatData()
//------------------------------------------------------------------------------
/**
 * Method used to write additional Smoother data to the MATLAB .mat file
 *
 * @return true on success
 */
 //------------------------------------------------------------------------------
bool Smoother::WriteAdditionalMatData()
{
   bool retval = true;

   StringArray dataDesc;

   DataBucket matBackComputed, matBackFilter;
   IntegerMap matBackComputedIndex, matBackFilterIndex;
   matBackComputed.SetInitialRealValue(NAN);
   matBackFilter.SetInitialRealValue(NAN);

   // populate backwards filter mat data
   for (UnsignedInt ii = 0U; ii < backwardFilterInfo.size(); ii++)
   {
      AddMatlabFilterData(backwardFilterInfo[ii], matBackFilter, matBackFilterIndex);

      if (backwardFilterInfo[ii].isObs)
         AddMatlabData(backwardFilterInfo[ii].measStat, matBackComputed, matBackComputedIndex);
   }

   // Add backward filter computed data
   matWriter->ClearData();
   AddMatData(matBackComputed);

   dataDesc = GetMatDataDescription(matBackComputed);
   matWriter->DescribeData(dataDesc);
   matWriter->WriteData("BackwardComputed");

   // Add backward filter data
   matWriter->ClearData();
   AddMatData(matBackFilter);

   dataDesc = GetMatDataDescription(matBackFilter);
   matWriter->DescribeData(dataDesc, iterationsTaken);
   matWriter->WriteData("BackwardFilter");

   return retval;
}
