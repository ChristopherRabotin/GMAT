//$Id$
//------------------------------------------------------------------------------
//                           RangeSkinAdapter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: 
// Created: May, 16 2019
/**
* A measurement adapter for c-band measurements
*/
//------------------------------------------------------------------------------

#include "RangeSkinAdapter.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include "ErrorModel.hpp"

//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_RANGE_CALCULATION

//------------------------------------------------------------------------------
// RangeSkinAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
RangeSkinAdapter::RangeSkinAdapter(const std::string & name) :
   GNRangeAdapter(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeSkinAdapter default constructor <%p>\n", this);
#endif

   typeName = "RangeSkin";
}

//------------------------------------------------------------------------------
// ~RangeSkinAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RangeSkinAdapter::~RangeSkinAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeSkinAdapter default destructor  <%p>\n", this);
#endif
}

//------------------------------------------------------------------------------
// RangeSkinAdapter(const RangeSkinAdapter& rsa)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rsa The adapter copied to make this one
 */
//------------------------------------------------------------------------------
RangeSkinAdapter::RangeSkinAdapter(const RangeSkinAdapter & rsa) : 
   GNRangeAdapter(rsa)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeSkinAdapter copy constructor   from <%p> to <%p>\n", &rsa, this);
#endif
}

//------------------------------------------------------------------------------
// GNRangeAdapter& operator=(const RangeSkinAdapter& rsa)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rsa The adapter copied to make this one match it
 *
 * @return This adapter made to look like rsa
 */
//------------------------------------------------------------------------------
RangeSkinAdapter & RangeSkinAdapter::operator=(const RangeSkinAdapter & rsa)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeSkinAdapter operator =   set <%p> = <%p>\n", this, &rsa);
#endif

   if (this != &rsa)
   {
      GNRangeAdapter::operator=(rsa);
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new adapter that matches this one
 *
 * @return A new adapter set to match this one
 */
//------------------------------------------------------------------------------
GmatBase * RangeSkinAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeSkinAdapter::Clone() clone this <%p>\n", this);
#endif

   return new RangeSkinAdapter(*this);
}

//------------------------------------------------------------------------------
// void computeBiasAndNoise(std::string& measType, Integer numTrip)
//------------------------------------------------------------------------------
/**
 * Computes bias and noise for the measurment strand
 *
 * @param measType the measurement type to apply for the corrections
 * @param numTrip number of trips within the strand
 */
 //------------------------------------------------------------------------------
void RangeSkinAdapter::ComputeBiasAndNoise(const std::string& measType, 
   const Integer numTrip)
{
   GNRangeAdapter::ComputeBiasAndNoise("Range", numTrip);
}

//------------------------------------------------------------------------------
// void applyBiasAndNoise(const std::string measType,
//   const RealArray corrections, const RealArray values)
//------------------------------------------------------------------------------
/**
 * Applies bias and noise to measurements and handles range based multiplier
 *
 * @param measType the measurement type to apply the corrections
 * @param corrections the corrections
 * @param values the measurment values
 */
 //------------------------------------------------------------------------------
void RangeSkinAdapter::ApplyBiasAndNoise(const std::string& measType, 
   const RealArray& corrections, const RealArray& values)
{
   GNRangeAdapter::ApplyBiasAndNoise("Range", corrections, values);
}



//------------------------------------------------------------------------------
// Real applyMultiplier(const std::string& useMeasType,
//   const Real factor, const GmatBase* obj)
//------------------------------------------------------------------------------
/**
* Resolves a multiplier based on the passed in measurment type
*
* @param useMeasType The measurement type to use
* @param factor The initial factor
* @param obj The GMAT object in the current path
*
* @return multiplier factor
*/
//------------------------------------------------------------------------------
Real RangeSkinAdapter::ApplyMultiplier(const std::string& useMeasType, 
   const Real factor, const GmatBase* obj)
{
   return RangeAdapterKm::ApplyMultiplier("Range", factor, obj);
}

//------------------------------------------------------------------------------
// void ComputeMeasurementBias(const std::string biasName, 
//                             const std::string measType, Integer numTrip)
//------------------------------------------------------------------------------
/**
* Get measurement bias
*
* @param biasName      Name of the measurement bias. In this case is "Bias"
* @param measType      Measurement type of this tracking data
* @param numTrip       Number of ways signal travel such as 1-way, 2-ways,
*                      or 3-ways
*
* @return              Value of bias
*/
//------------------------------------------------------------------------------
void RangeSkinAdapter::ComputeMeasurementBias(const std::string biasName,
   const std::string measType, 
                                              Integer numTrip)
{
   TrackingDataAdapter::ComputeMeasurementBias(biasName, "Range_Skin", numTrip);
}

//------------------------------------------------------------------------------
// void ComputeMeasurementNoiseSigma(const std::string noiseSigmaName, 
//                                   const std::string measType, Integer numTrip)
//------------------------------------------------------------------------------
/**
* Get measurement noise sigma
*
* @param noisSigmaName      Name of the measurement noise sigma. In this case is
*                           "NoisSigma"
* @param measType           Measurement type of this tracking data
* @param numTrip            NumTrip shown number of ways signal travel such as
*                           1-way, 2-ways, or 3-ways
*
* @return                   Value of noise sigma
*/
//------------------------------------------------------------------------------
void RangeSkinAdapter::ComputeMeasurementNoiseSigma(const std::string noiseSigmaName,
   const std::string measType, Integer numTrip)
{
   TrackingDataAdapter::ComputeMeasurementNoiseSigma(noiseSigmaName, "Range_Skin", numTrip);
}

//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(GmatBase* obj,
//       Integer id)
//------------------------------------------------------------------------------
/**
 * Computes measurement derivatives for a given parameter on a given object
 *
 * @param obj The object that has the w.r.t. parameter
 * @param id  The ID of the w.r.t. parameter
 *
 * @return The derivative vector
 */
 //------------------------------------------------------------------------------
const std::vector<RealArray>& RangeSkinAdapter::CalculateMeasurementDerivatives(
   GmatBase *obj, Integer id)
{
   if (!calcData)
      throw MeasurementException("Measurement derivative data was requested "
         "for " + instanceName + " before the measurement was set");

   Integer parameterID;
   if (id > 250)
      parameterID = id - obj->GetType() * 250;
   else
      parameterID = id;

   std::string paramName = obj->GetParameterText(parameterID);
   
   if (paramName == "Bias") {
      // Clear derivative variable
      for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
         theDataDerivatives[i].clear();
      theDataDerivatives.clear();

      if (((ErrorModel*)obj)->GetStringParameter("Type") == "Range_Skin")
         theDataDerivatives = calcData->CalculateMeasurementDerivatives(obj, id);
      else
      {
         Integer size = obj->GetEstimationParameterSize(id);
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);
      }
      
   }
   else 
   {
      RangeAdapterKm::CalculateMeasurementDerivatives(obj, id);
   }

   return theDataDerivatives;
}