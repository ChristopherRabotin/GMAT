//$Id$
//------------------------------------------------------------------------------
//                           DopplerAdapter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: Sept 30, 2014
/**
 * A measurement adapter for DSN Doppler
 */
//------------------------------------------------------------------------------

#include "DopplerAdapter.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "SignalBase.hpp"
#include "Transponder.hpp"


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_DERIVATIVE_CALCULATION
#define DEBUG_DOPPLER_CALCULATION

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
DopplerAdapter::PARAMETER_TEXT[DopplerAdapterParamCount - RangeAdapterKmParamCount] =
{
   "DopplerCountInterval",
};


const Gmat::ParameterType
DopplerAdapter::PARAMETER_TYPE[DopplerAdapterParamCount - RangeAdapterKmParamCount] =
{
   Gmat::REAL_TYPE,
};




//------------------------------------------------------------------------------
// DopplerAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
DopplerAdapter::DopplerAdapter(const std::string& name) :
   RangeAdapterKm         (name),
   adapterS               (NULL),
   turnaround             (1.0),
   uplinkFreqE            (1.0e9),
   freqBandE              (1),
   dopplerCountInterval   (1.0)        // 1 second
{
   typeName = "Doppler";              // change type name from "RangeKm" to "Doppler"
}


//------------------------------------------------------------------------------
// ~DopplerAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DopplerAdapter::~DopplerAdapter()
{
   if (adapterS)
      delete adapterS;
}


//------------------------------------------------------------------------------
// DopplerAdapter(const DopplerAdapter& da)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param da The adapter copied to make this one
 */
//------------------------------------------------------------------------------
DopplerAdapter::DopplerAdapter(const DopplerAdapter& da) :
   RangeAdapterKm         (da),
   adapterS               (NULL),
   turnaround             (da.turnaround),
   uplinkFreqE            (da.uplinkFreqE),
   freqBandE              (da.freqBandE),
   dopplerCountInterval   (da.dopplerCountInterval)
{
}


//------------------------------------------------------------------------------
// DopplerAdapter& operator=(const DopplerAdapter& da)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param da The adapter copied to make this one match it
 *
 * @return This adapter made to look like da
 */
//------------------------------------------------------------------------------
DopplerAdapter& DopplerAdapter::operator=(const DopplerAdapter& da)
{
   if (this != &da)
   {
      RangeAdapterKm::operator=(da);

      turnaround           = da.turnaround;
      uplinkFreqE          = da.uplinkFreqE;
      freqBandE            = da.freqBandE;
      dopplerCountInterval = da.dopplerCountInterval;

      adapterS             = NULL;
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
GmatBase* DopplerAdapter::Clone() const
{
   return new DopplerAdapter(*this);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the script name for the parameter
 *
 * @param id The id of the parameter
 *
 * @return The script name
 */
//------------------------------------------------------------------------------
std::string DopplerAdapter::GetParameterText(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < DopplerAdapterParamCount)
      return PARAMETER_TEXT[id - RangeAdapterKmParamCount];
   return RangeAdapterKm::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable parameter
 *
 * @param str The string used for the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer DopplerAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = RangeAdapterKmParamCount; i < DopplerAdapterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - RangeAdapterKmParamCount])
         return i;
   }
   return RangeAdapterKm::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a specified parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType DopplerAdapter::GetParameterType(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < DopplerAdapterParamCount)
      return PARAMETER_TYPE[id - RangeAdapterKmParamCount];

   return RangeAdapterKm::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description for a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The description string
 */
//------------------------------------------------------------------------------
std::string DopplerAdapter::GetParameterTypeString(const Integer id) const
{
   return MeasurementModelBase::PARAM_TYPE_STRING[GetParameterType(id)];
}



//------------------------------------------------------------------------------
// std::string GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real DopplerAdapter::GetRealParameter(const Integer id) const
{
   if (id == DOPPLER_COUNT_INTERVAL)
      return dopplerCountInterval;

   return RangeAdapterKm::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real DopplerAdapter::SetRealParameter(const Integer id, const Real value)
{
   if (id == DOPPLER_COUNT_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: Doppler count interval has a nonpositive value\n");

      dopplerCountInterval = value;
      return dopplerCountInterval;
   }

   return RangeAdapterKm::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param label The name for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real DopplerAdapter::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real DopplerAdapter::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}



//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string& oldName,
//       const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Method used to rename reference objects
 *
 * @param type The type of the object that is renamed
 * @param oldName The old object name
 * @param newName The new name
 *
 * @return true if a rename happened, false if not
 */
//------------------------------------------------------------------------------
bool DopplerAdapter::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = RangeAdapterKm::RenameRefObject(type, oldName, newName);

   // Handle additional renames specific to this adapter

   return retval;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the adapter for use
 *
 * @return true if the initialization succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool DopplerAdapter::Initialize()
{
   bool retval = false;

   if (RangeAdapterKm::Initialize())
   {
      retval = true;

      // @todo: initialize all needed variables
      // 1. Create S-path measurement model;

      adapterS =  new RangeAdapterKm("adapterS");
      *adapterS = *this;

      // 2. Set all needed parameters
      // 2.1. Set for participantLists
      for (UnsignedInt i = 0; i < participantLists.size(); ++i)
      {
         MessageInterface::ShowMessage("i = %d\n", i);
         for (UnsignedInt j = 0; j < participantLists[i]->size(); ++j)
         {
            MessageInterface::ShowMessage("j = %d\n", j);
            MessageInterface::ShowMessage("adapterS->SetStringParameter('SignalPath', '%s', %d)\n", participantLists[i]->at(j).c_str(), i);
            adapterS->SetStringParameter("SignalPath", participantLists[i]->at(j), i);
         }
      }
      // 2.2. Set for all Reference objects
      StringArray refObjNames = adapterS->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
      MessageInterface::ShowMessage(" Reference object names:\n");
      for (UnsignedInt i = 0; i < refObjNames.size(); ++i)
         MessageInterface::ShowMessage(" .%s\n", refObjNames[i].c_str());

      ObjectArray objs = GetRefObjectArray(Gmat::UNKNOWN_OBJECT);
      MessageInterface::ShowMessage(" Reference objects: size = %d\n", objs.size());
      for (UnsignedInt i = 0; i < objs.size(); ++i)
         MessageInterface::ShowMessage(" .<%p,%s>\n", objs[i], objs[i]->GetName().c_str());

      for (UnsignedInt i = 0; i < refObjNames.size(); ++i)
      {
         // search for object with name refObjNames[i]
         GmatBase* obj = NULL;
         for (UnsignedInt j = 0; j < objs.size(); ++j)
         {
            if (objs[j]->GetName() == refObjNames[i])
            {
               obj = objs[j];
               break;
            }
         }

         // set reference object obj to adapterS
         adapterS->SetRefObject(obj, obj->GetType(), obj->GetName());
      }
      
      // 2.3. Set propagator
      if (thePropagator)
      {
         adapterS->SetPropagator(thePropagator);
      }

      adapterS->Initialize();
   }

   return retval;
}


//------------------------------------------------------------------------------
// const MeasurementData& CalculateMeasurement(bool withEvents,
//       ObservationData* forObservation, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
 * Computes the measurement associated with this adapter
 *
 * @note: The parameters associated with this call will probably be removed;
 * they are here to support compatibility with the old measurement models
 *
 * @param withEvents Flag indicating is the light time solution should be
 *                   included
 * @param forObservation The observation data associated with this measurement
 * @param rampTB Ramp table for a ramped measurement
 *
 * @return The computed measurement data
 */
//------------------------------------------------------------------------------
const MeasurementData& DopplerAdapter::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTable)
{
   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("DopplerAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
   #endif

   // 1. Set ramp table and observation data for adapter before doing something
   rampTB = rampTable;
   obsData = forObservation;


   // 3. Compute for End path
   // 3.1. Propagate all space objects to time tm
   // This step is not needed due to measurement time tm is set to t3RE
   
   // 3.2. Compute range in km for End Path
   RangeAdapterKm::CalculateMeasurement(withEvents, forObservation, rampTB);
   measDataE = cMeasurement;
   
   // 3.3. Specify uplink frequency
   // Note that: In the current version, only one signal path is used in AdapterConfiguration. Therefore, path index is 0 
   uplinkFreqE = calcData->GetUplinkFrequency(0, rampTB);
   freqBandE = calcData->GetUplinkFrequencyBand(0, rampTB);

   
   // 3. Compute for Start path
   // 3.1. Propagate all space objects to time tm-Tc
   MeasureModel* measModel = adapterS->GetMeasurementModel();
   std::vector<SignalBase*> paths = measModel->GetSignalPaths();
   for (UnsignedInt i = 0; i < paths.size(); ++i)
   {
      SignalBase* leg = paths[i];
      while (leg != NULL)
         leg->MoveToEpoch(cMeasurement.epoch, true, true);
      leg = leg->GetNext();
   }

   // 3.2. Compute range in km for End Path
   adapterS->CalculateMeasurement(withEvents, forObservation, rampTB);
   measDataS = cMeasurement;

   // 3.3. Specify uplink frequency
   // Note that: In the current version, only one signal path is used in AdapterConfiguration. Therefore, path index is 0 
   uplinkFreq = measModel->GetUplinkFrequency(0, rampTB);
   freqBand   = measModel->GetUplinkFrequencyBand(0, rampTB);

      
   // 4. Convert range from km to Hz and store in cMeasurement:
   Real dtS, dtE, dtdt, t1TE, t3RE;
   Real interval = dopplerCountInterval;

   for (UnsignedInt i = 0; i < paths.size(); ++i)          // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths.
   {
      // 4.1. Calculate total turn around ratio for paths[i]
      // Note: Total turn around ratio equals product of transponder turn around ratio for all spacecrafts
      SignalBase* leg = paths[i];
      turnaround = 1.0;
      while (leg != NULL)
      {
         SignalData* sd = leg->GetSignalDataObject();
         SpacePoint* sp = sd->rNode;
         if (sp->IsOfType(Gmat::SPACECRAFT))
         {
            ObjectArray hw = sp->GetRefObjectArray(Gmat::HARDWARE);
            for (UnsignedInt j = 0; j < hw.size(); ++j)
            {
               if (hw[j]->IsOfType("Transponder"))
               {
                  turnaround *= ((Transponder*)hw[j])->GetRealParameter("TurnAroundRatio");
                  break;
               }
            }// for j
         }

         leg = leg->GetNext();
      }// while

      // 4.2. Specify multiplier for S-path and E-path
      multiplierS = turnaround*(uplinkFreq*1.0e6)/interval;
      multiplierE = turnaround*(uplinkFreqE*1.0e6)/interval;

      // 4.3. Time travel for S-path and E-path
      dtS = measDataS.value[i] / (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);
      dtE = measDataE.value[i] / (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);
      dtdt = dtE - dtS;
      t3RE = measDataE.epoch;
      t1TE = t3RE - dtE;

      // 4.4. Calculate Frequency Doppler Shift
      if (rampTB != NULL)
      {
         Integer errnum;
         try
         {
//           currentMeasurement.value[0] = (M2R*IntegralRampedFrequency(t3RE, interval) - turnaround*IntegralRampedFrequency(t1TE, interval + dtS-dtE))/ interval;
            
            cMeasurement.value[i] = - turnaround*IntegralRampedFrequency(t1TE, interval - dtdt, errnum)/ interval;
         } catch (MeasurementException exp)
         {
            cMeasurement.value[i] = 0.0;                     // It has no C-value due to the failure of calculation of IntegralRampedFrequency()
            cMeasurement.uplinkFreq = uplinkFreqE;           // unit: Hz
            cMeasurement.uplinkBand = freqBandE;
            cMeasurement.dopplerCountInterval = interval;    // unit: second
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = "R";
//          currentMeasurement.feasibilityValue is set to elevation angle as shown in section 15
         
            if ((errnum == 2)||(errnum == 3))
               throw exp;
         }
      }
      else
      {       
         cMeasurement.value[i] = -turnaround*uplinkFreq*(interval - dtdt)/interval;
      }
      
      cMeasurement.uplinkFreq = uplinkFreq*1.0e6;         // convert Mhz to Hz due cMeasurement.uplinkFreq's unit is Hz
      cMeasurement.uplinkBand = freqBand;
      cMeasurement.dopplerCountInterval = interval;


      #ifdef DEBUG_DOPPLER_CALCULATION
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("====  DopplerAdapter: Range Calculation for Measurement Data %dth  \n", i);
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("      . Measurement type            : <%s>\n", measurementType.c_str());
         MessageInterface::ShowMessage("      . Noise adding option         : %s\n", (addNoise?"true":"false"));
         MessageInterface::ShowMessage("      . Doppler count interval      : %.12lf seconds\n", interval);
         MessageInterface::ShowMessage("      . Real travel time for S-path : %.12lf seconds\n", dtS);
         MessageInterface::ShowMessage("      . Real travel time for E-path : %.12lf seconds\n", dtE);
         MessageInterface::ShowMessage("      . Multiplier factor for S-path: %.12lf\n", multiplierS);
         MessageInterface::ShowMessage("      . Multiplier factor for E-path: %.12lf\n", multiplierE);
         MessageInterface::ShowMessage("      . C-value (with noise)        : %.12lf Hz\n", cMeasurement.value[i]);
         MessageInterface::ShowMessage("      . Measurement epoch A1Mjd     : %.12lf\n", cMeasurement.epoch); 
         MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible?"feasible":"unfeasible"));
         MessageInterface::ShowMessage("      . Feasibility reason          : %s\n", cMeasurement.unfeasibleReason.c_str());
         MessageInterface::ShowMessage("      . Elevation angle             : %.12lf degree\n", cMeasurement.feasibilityValue);
         MessageInterface::ShowMessage("===================================================================\n");
      #endif

   }

   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("DopplerAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) exit\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
   #endif

   return cMeasurement;
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
const std::vector<RealArray>& DopplerAdapter::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      Integer parmId = GetParmIdFromEstID(id, obj);
      MessageInterface::ShowMessage("Enter DopplerAdapter::CalculateMeasurementDerivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n", obj->GetName().c_str(), id, parmId, cMeasurement.epoch);
   #endif

   
   // Compute measurement derivatives in km for E-path:
   std::vector<RealArray> derivativesE = RangeAdapterKm::CalculateMeasurementDerivatives(obj, id);

   // Compute measurement derivatives in km for S-path:
   std::vector<RealArray> derivativesS = adapterS->CalculateMeasurementDerivatives(obj, id);

   // Convert measurement derivatives from km/s to Hz
   for (UnsignedInt i = 0; i < derivativesE.size(); ++i)
   {
      for (UnsignedInt j = 0; j < derivativesE[i].size(); ++j)
      {
         theDataDerivatives[i][j] = derivativesE[i][j] * multiplierE - derivativesS[i][j] * multiplierS;
      }
   }
   
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      {
         MessageInterface::ShowMessage("Derivative for path %dth:\n", i);
         MessageInterface::ShowMessage("[");
         for (UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
         {
            MessageInterface::ShowMessage("    %.12lf", theDataDerivatives[i][j]);
            MessageInterface::ShowMessage("%s", ((j == theDataDerivatives[i].size()-1)?"":","));
         }
         MessageInterface::ShowMessage("]\n");
      }
      
   #endif

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Exit DopplerAdapter::CalculateMeasurementDerivatives():\n");
   #endif

   return theDataDerivatives;
}


//------------------------------------------------------------------------------
// bool WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Method to write measurements
 *
 * @todo Implement this method
 *
 * @return true if written, false if not
 */
//------------------------------------------------------------------------------
bool DopplerAdapter::WriteMeasurements()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool WriteMeasurement(Integer id)
//------------------------------------------------------------------------------
/**
 * Method to write a specific measurement
 *
 * @todo Implement this method
 *
 * @param id ID of the parameter to write
 *
 * @return true if written, false if not
 */
//------------------------------------------------------------------------------
bool DopplerAdapter::WriteMeasurement(Integer id)
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// Integer HasParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
 * Checks to see if adapter has covariance data for a specified parameter ID
 *
 * @param paramID The parameter ID
 *
 * @return Size of the covariance data that is available
 */
//------------------------------------------------------------------------------
Integer DopplerAdapter::HasParameterCovariances(Integer parameterId)
{
   Integer retval = 0;

   return retval;
}


//------------------------------------------------------------------------------
// Integer GetEventCount()
//------------------------------------------------------------------------------
/**
 * Returns the number of light time solution events in the measurement
 *
 * @return The event count
 */
//------------------------------------------------------------------------------
Integer DopplerAdapter::GetEventCount()
{
   Integer retval = 0;

   return retval;
}


//------------------------------------------------------------------------------
// void SetCorrection(const std::string& correctionName,
//       const std::string& correctionType)
//------------------------------------------------------------------------------
/**
 * Passes a correction name into the owned CoreMeasurement
 *
 * @param correctionName The name of the correction
 * @param correctionType The type of correction
 *
 * @note This information originally was not passed via SetStringParameter
 *       because it wasn't managed by scripting on MeasurementModels.  It was
 *       managed in the TrackingSystem code.  Since it is now scripted on the
 *       measurement -- meaning on the adapter -- this code should move into the
 *       Get/SetStringParameter methods.  It is included here !!TEMPORARILY!!
 *       to get a scripted adapter functioning in GMAT Nav.
 */
//------------------------------------------------------------------------------
void DopplerAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   RangeAdapterKm::SetCorrection(correctionName, correctionType);
}
