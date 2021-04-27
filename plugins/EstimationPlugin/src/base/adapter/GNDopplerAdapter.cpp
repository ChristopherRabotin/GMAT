//$Id$
//------------------------------------------------------------------------------
//                           GNDopplerAdapter
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
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: May 7, 2015
/**
 * A measurement adapter for GN Doppler
 */
//------------------------------------------------------------------------------

#include "GNDopplerAdapter.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "SignalBase.hpp"
#include "Transponder.hpp"
#include "PropSetup.hpp"
#include "RandomNumber.hpp"
#include "ErrorModel.hpp"
#include <sstream>

//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_DERIVATIVE_CALCULATION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZE
//#define DEBUG_DOPPLER_CALCULATION

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
GNDopplerAdapter::PARAMETER_TEXT[GNDopplerAdapterParamCount - RangeAdapterKmParamCount] =
{
   "DopplerCountInterval",
};


const Gmat::ParameterType
GNDopplerAdapter::PARAMETER_TYPE[GNDopplerAdapterParamCount - RangeAdapterKmParamCount] =
{
   Gmat::REAL_TYPE,
};




//------------------------------------------------------------------------------
// GNDopplerAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
GNDopplerAdapter::GNDopplerAdapter(const std::string& name) :
   RangeAdapterKm         (name),
   adapterS               (NULL),
   turnaround             (1.0),
   uplinkFreqE            (1.0e3),      // 1000 MHz
   freqBandE              (1),
   dopplerCountInterval   (1.0),        // unit: 1 second
   multiplierS            (1.0),        // unit: 1/1second
   multiplierE            (1.0)         // unit: 1/1second
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNDopplerAdapter default constructor <%p>\n", this);
#endif
   typeName = "RangeRate";  //"Doppler_RangeRate";              // change type name from "RangeKm" to ("Doppler_RangeRate") "RangeRate"
}


//------------------------------------------------------------------------------
// ~GNDopplerAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GNDopplerAdapter::~GNDopplerAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNDopplerAdapter default destructor <%p>\n", this);
#endif

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
GNDopplerAdapter::GNDopplerAdapter(const GNDopplerAdapter& da) :
   RangeAdapterKm         (da),
   //adapterS               (NULL),
   turnaround             (da.turnaround),
   uplinkFreqE            (da.uplinkFreqE),
   freqBandE              (da.freqBandE),
   dopplerCountInterval   (da.dopplerCountInterval),
   multiplierS            (da.multiplierS),
   multiplierE            (da.multiplierE)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNDopplerAdapter copy constructor   from <%p> to <%p>\n", &da, this);
#endif
   if (adapterS)
   {
      delete adapterS;
      adapterS = NULL;
   }
   if (da.adapterS)
      adapterS = (RangeAdapterKm*)da.adapterS->Clone();
}


//------------------------------------------------------------------------------
// GNDopplerAdapter& operator=(const GNDopplerAdapter& da)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param da The adapter copied to make this one match it
 *
 * @return This adapter made to look like da
 */
//------------------------------------------------------------------------------
GNDopplerAdapter& GNDopplerAdapter::operator=(const GNDopplerAdapter& da)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNDopplerAdapter operator =   set <%p> = <%p>\n", this, &da);
#endif

   if (this != &da)
   {
      RangeAdapterKm::operator=(da);

      turnaround           = da.turnaround;
      uplinkFreqE          = da.uplinkFreqE;
      freqBandE            = da.freqBandE;
      dopplerCountInterval = da.dopplerCountInterval;
      multiplierS          = da.multiplierS;
      multiplierE          = da.multiplierE;

      if (adapterS)
      {
         delete adapterS;
         adapterS = NULL;
      }
      if (da.adapterS)
         adapterS = (RangeAdapterKm*)da.adapterS->Clone();
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss The pointer
 */
//------------------------------------------------------------------------------
void GNDopplerAdapter::SetSolarSystem(SolarSystem *ss)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("GNDopplerAdapter<%p>::SetSolarSystem('%s')\n", this, ss->GetName().c_str()); 
#endif

   adapterS->SetSolarSystem(ss);

   RangeAdapterKm::SetSolarSystem(ss);
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
GmatBase* GNDopplerAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNDopplerAdapter::Clone()   clone this <%p>\n", this);
#endif

   return new GNDopplerAdapter(*this);
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
std::string GNDopplerAdapter::GetParameterText(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < GNDopplerAdapterParamCount)
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
Integer GNDopplerAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = RangeAdapterKmParamCount; i < GNDopplerAdapterParamCount; i++)
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
Gmat::ParameterType GNDopplerAdapter::GetParameterType(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < GNDopplerAdapterParamCount)
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
std::string GNDopplerAdapter::GetParameterTypeString(const Integer id) const
{
   return MeasurementModelBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool GNDopplerAdapter::SetStringParameter(const Integer id, const std::string& value)
{
   // Note that: measurement type of adapter is always ("Doppler_RangeRate") "RangeRate", so it does not need to change
   bool retval = true;
   if ((id != MEASUREMENT_TYPE)&&(id != SIGNAL_PATH))
      retval = adapterS->SetStringParameter(id, value);

   retval = RangeAdapterKm::SetStringParameter(id, value) && retval;

   return retval; 
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param id The ID for the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool GNDopplerAdapter::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   bool retval = adapterS->SetStringParameter(id, value, index);
   retval = RangeAdapterKm::SetStringParameter(id, value, index) && retval;

   return retval;
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The scriptable name of the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool GNDopplerAdapter::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param label The scriptable name of the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool GNDopplerAdapter::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// bool SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Integer GNDopplerAdapter::SetIntegerParameter(const Integer id, const Integer value)
{
   adapterS->SetIntegerParameter(id, value);
   return RangeAdapterKm::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Integer GNDopplerAdapter::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
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
Real GNDopplerAdapter::GetRealParameter(const Integer id) const
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
Real GNDopplerAdapter::SetRealParameter(const Integer id, const Real value)
{
   if (id == DOPPLER_COUNT_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: Doppler count interval has a nonpositive value\n");

      dopplerCountInterval = value;
      return dopplerCountInterval;
   }

   bool retval = adapterS->SetRealParameter(id, value);
   retval = RangeAdapterKm::SetRealParameter(id, value) && retval;

   return retval;
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
Real GNDopplerAdapter::GetRealParameter(const std::string &label) const
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
Real GNDopplerAdapter::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


bool GNDopplerAdapter::SetBooleanParameter(const Integer id, const bool value)
{
   // Note: for Start path, AddNoise always is set to false due to it calculation
   bool retval = true;
   if (id == ADD_NOISE)
      retval = adapterS->SetBooleanParameter(id, false);
   else
      retval = adapterS->SetBooleanParameter(id, value);

   retval = RangeAdapterKm::SetBooleanParameter(id, value) && retval;

   return retval;
}


bool GNDopplerAdapter::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string& oldName,
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
bool GNDopplerAdapter::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   // Handle additional renames specific to this adapter
   bool retval = adapterS->RenameRefObject(type, oldName, newName);
   retval = RangeAdapterKm::RenameRefObject(type, oldName, newName) && retval;

   return retval;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets pointers to the model's reference objects
 *
 * @param obj The object pointer
 * @param type The type of the object (not used)
 * @param name The name of the object  (not used)
 *
 * @return true if the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool GNDopplerAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name)
{
   bool retval = adapterS->SetRefObject(obj, type, name);
   retval = RangeAdapterKm::SetRefObject(obj, type, name) && retval;

   return retval; 
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the pointers for the reference object
 *
 * @param obj The object pointer
 * @param type The type of the object (not used)
 * @param name The name of the object  (not used)
 * @param index Index for the object's location
 *
 * @return true if the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool GNDopplerAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name, const Integer index)
{
   bool retval = adapterS->SetRefObject(obj, type, name, index);
   retval = RangeAdapterKm::SetRefObject(obj, type, name, index) && retval;

   return retval;
}

//------------------------------------------------------------------------------
// bool SetMeasurement(MeasureModel* meas)
//------------------------------------------------------------------------------
/**
 * Sets the measurement model pointer
 *
 * @param meas The pointer
 *
 * @return true if set, false if not
 */
//------------------------------------------------------------------------------
bool GNDopplerAdapter::SetMeasurement(MeasureModel* meas)
{
   return RangeAdapterKm::SetMeasurement(meas);
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* ps)
//------------------------------------------------------------------------------
/**
 * Passes a propagator to the adapter for use in light time iterations.  The
 * propagator is cloned so that propagation of single spacecraft can be
 * performed.
 *
 * @param ps The PropSetup that is being set
 *
 * @todo The current call takes a single propagator.  Once the estimation system
 *       supports multiple propagators, this should be changed to a vector of
 *       PropSetup objects.
 */
//------------------------------------------------------------------------------
void GNDopplerAdapter::SetPropagators(std::vector<PropSetup*> *ps,
      std::map<std::string, StringArray> *spMap)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting propagator to %p in "
            "GNDopplerAdapter\n", ps);
   #endif

   adapterS->SetPropagators(ps, spMap);
   RangeAdapterKm::SetPropagators(ps, spMap);
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
* Passes the transient force vector into the adapter
*
* The transient force vector is a set of models used in GMAT's ODEModel for
* affects that are turned on and off over the course of a mission.  An example
* of a transient force is a finite burn, which is toggled by the
* BeginFiniteBurn and EndFiniteBurn commands.  These components are only used
* by commands that need them.  Typical usage is found in the propagation
* enabled commands.
*
* @param tf The vector of transient forces
*/
//------------------------------------------------------------------------------
void GNDopplerAdapter::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
    GetMeasurementModel()->SetTransientForces(tf);
    adapterS->SetTransientForces(tf);
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
bool GNDopplerAdapter::Initialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("GNDopplerAdapter::Initialize() <%p> start\n", this);
#endif

   bool retval = false;

   if (RangeAdapterKm::Initialize())
   {
      // @todo: initialize all needed variables

      retval = adapterS->Initialize();
   }

#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("GNDopplerAdapter::Initialize() <%p> exit\n", this);
#endif
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
const MeasurementData& GNDopplerAdapter::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTable,
      bool forSimulation)
{
   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("GNDopplerAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
   #endif

   // 1. Set value for local variables
   rampTB = rampTable;
   Integer err = 0;
   if (rampTable != NULL)
   {
      BeginEndIndexesOfRampTable(err);
   }

   obsData = forObservation;
   // 1.2. Reset value for doppler count interval
   if (obsData)
      dopplerCountInterval = obsData->dopplerCountInterval;          // unit: second


   // 2. Compute for End path
   // 2.1. Propagate all space objects to time tm
   // This step is not needed due to measurement time tm is set to t3RE
   // 2.2. Compute range in km for End Path
   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for E-Path...\n");
   #endif
   bool addNoiseOption  = addNoise;
   bool addBiasOption   = addBias;
   bool rangeOnlyOption = rangeOnly; 

   addNoise = false;
   addBias = false;
   rangeOnly = true;
   RangeAdapterKm::CalculateMeasurement(withEvents, forObservation, rampTB, forSimulation);
   measDataE = cMeasurement;
   measDataE.value[0] = (measDataE.value[0] - 2 * GetIonoCorrection());
   measDataE.correction[0] = (measDataE.correction[0] - 2 * GetIonoCorrection());
   
   addNoise = addNoiseOption;
   addBias = addBiasOption;
   rangeOnly = rangeOnlyOption;
   
   // 2.3. Specify uplink frequency
   // Note that: In the current version, only one signal path is used in AdapterConfiguration. Therefore, path index is 0 
   uplinkFreqE       = calcData->GetUplinkFrequency(0, rampTB);                              // unit: MHz
   uplinkFreqAtRecei = calcData->GetUplinkFrequencyAtReceivedEpoch(0, rampTB);               // unit: MHz   // its value is the frequency at measurement epoch
   freqBandE         = calcData->GetUplinkFrequencyBand(0, rampTB);
   

   // 3. Compute for Start path
   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for S-Path...\n");
   #endif
   // 3.1. Measurement time is the same as the one for End-path
   GmatTime tm = cMeasurement.epochGT;       // Get measurement time
   ObservationData* obData = NULL;
   if (forObservation)
      obData = new ObservationData(*forObservation);
   else
      obData = new ObservationData();
   obData->epochGT = tm;
   obData->epoch   = tm.GetMjd();
   
   // Set doppler count interval to MeasureModel object due to the Start-path
   // is measured earlier by number of seconds shown in doppler count interval
   adapterS->GetMeasurementModel()->SetCountInterval(dopplerCountInterval);
   // For Start-path, range calculation does not add bias and noise to calculated value
   // Note that: default option is no adding noise
   adapterS->AddBias(false);
   adapterS->AddNoise(false);
   adapterS->SetRangeOnly(true);
   
   adapterS->CalculateMeasurement(withEvents, obData, rampTB, forSimulation);
   
   if (obData)
   {
      delete obData;
      obData = NULL;
   }

   measDataS = adapterS->GetMeasurement();
   // measDataS.value[0] = measDataS.value[0] / adapterS->GetMultiplierFactor();                                         // convert to full range in km
   measDataS.value[0] = (measDataS.value[0] - 2 * adapterS->GetIonoCorrection()) / adapterS->GetMultiplierFactor();      // convert to full range in km
   measDataS.correction[0] = (measDataS.correction[0] - 2 * adapterS->GetIonoCorrection()) / adapterS->GetMultiplierFactor();      // convert to full range in km

   // Set value for isFeasible, feasibilityValue, and unfeasibleReason for measurement
   if ((measDataE.unfeasibleReason.at(0) == 'B') || (measDataS.unfeasibleReason.at(0) == 'B'))
   {
      if (measDataE.unfeasibleReason.at(0) == 'B')
         cMeasurement.unfeasibleReason = measDataE.unfeasibleReason + "E";
      else
      {
         cMeasurement.unfeasibleReason = measDataS.unfeasibleReason + "S";
         cMeasurement.isFeasible = false;
         cMeasurement.feasibilityValue = measDataS.feasibilityValue;
      }
   }

   // 3.2. Specify uplink frequency and band for Start path
   // Note that: In the current version, only one signal path is used in AdapterConfiguration. Therefore, path index is 0 
   uplinkFreq        = adapterS->GetMeasurementModel()->GetUplinkFrequency(0, rampTB);
   freqBand          = adapterS->GetMeasurementModel()->GetUplinkFrequencyBand(0, rampTB);

   // 4. Convert range from km to Hz and store in cMeasurement:
   Real dtS, dtE, dtdt;
   
   std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   for (UnsignedInt i = 0; i < paths.size(); ++i)          // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths.
   {
      // 4.0. Calculate total turn around ratio for paths[i]
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
                  // Get turn around ratio from spacrcraft transponder
                  Real ratio = ((Transponder*)hw[j])->GetTurnAroundRatio();
                  // Acummulate all turn around ratio
                  turnaround = turnaround * ratio;
                  break;
               }
            }// for j
         }

         // move to the next leg
         leg = leg->GetNext();
      }// while

      // 4.2. Specify multiplier for S-path and E-path
      multiplierS = 1/dopplerCountInterval;             // Its value is 1/Tc
      multiplierE = multiplierS;                        // Its value is 1/Tc

      // 4.1. Calculate GN Doppler w/o noise and bias
      dtS  = measDataS.value[0];              // real travel length for S-path   (unit: Km)
      dtE  = measDataE.value[0];              // real travel length for S-path   (unit: Km)
      
      if (USE_TAYLOR_SERIES || USE_CHEBYSHEV_DIFFERENCE)
      {
         dtdt = 0;
         for (UnsignedInt j = 0; j < measDataS.rangeVecs.size() && j < measDataE.rangeVecs.size(); j++)
         {
            Rvector3 signalVecS = *measDataS.rangeVecs[j] / adapterS->GetMultiplierFactor();
            Rvector3 signalVecE = *measDataE.rangeVecs[j];
            Rvector3 delta;

            if (USE_CHEBYSHEV_DIFFERENCE)
            {
               Rvector3 bodyDelta;
               Rvector3 chebyDelta;

               SpacePoint *tBody = measDataS.tBodies[j];
               SpacePoint *rBody = measDataS.rBodies[j];

               Rvector3 deltaR = *measDataE.rLocs[j] - *measDataS.rLocs[j];
               Rvector3 deltaT = *measDataE.tLocs[j] - *measDataS.tLocs[j];

               if (tBody->IsOfType(Gmat::CELESTIAL_BODY))
               {
                  bodyDelta = ((CelestialBody*) tBody)->GetPositionDeltaSSB(measDataS.tPrecTimes[j], measDataE.tPrecTimes[j]);
                  chebyDelta += -bodyDelta;
               }
               else
               {
                  throw MeasurementException("Unable to calculate Chebyshev difference for \"" +
                         tBody->GetName() + "\", the central body of each signal participant "
                         "must be a CelestialBody for Chebyshev differencing.");
               }

               if (rBody->IsOfType(Gmat::CELESTIAL_BODY))
               {
                  bodyDelta = ((CelestialBody*) rBody)->GetPositionDeltaSSB(measDataS.rPrecTimes[j], measDataE.rPrecTimes[j]);
                  chebyDelta += bodyDelta;
               }
               else
               {
                  throw MeasurementException("Unable to calculate Chebyshev difference for \"" +
                         rBody->GetName() + "\", the central body of each signal participant "
                         "must be a CelestialBody for Chebyshev differencing.");
               }

               delta = chebyDelta + deltaR - deltaT;
            }
            else
            {
               delta = signalVecE - signalVecS;
            }

            dtdt += PathMagnitudeDelta(signalVecS, delta);
         }
         dtdt += measDataE.correction[0] - measDataS.correction[0];
      }
      else
      {
         dtdt = dtE - dtS;                       // real travel difference          (unit: Km)
      }

      cMeasurement.value[i] = dtdt/dopplerCountInterval;  // GN doppler                      (unit: km/s)

      // Update media corrections
      cMeasurement.ionoCorrectValue = -(GetIonoCorrection() - adapterS->GetIonoCorrection()) / dopplerCountInterval;
      cMeasurement.tropoCorrectValue = (GetTropoCorrection() - adapterS->GetTropoCorrection()) / dopplerCountInterval;

      cMeasurement.uplinkFreq = uplinkFreq*1.0e6;                       // convert Mhz to Hz due cMeasurement.uplinkFreq's unit is Hz
      cMeasurement.uplinkFreqAtRecei = uplinkFreqAtRecei*1.0e6;         // convert Mhz to Hz due cMeasurement.uplinkFreqAtRecei's unit is Hz
      cMeasurement.uplinkBand = freqBand;
      cMeasurement.dopplerCountInterval = dopplerCountInterval;
      
      // 4.2. Add noise and bias if possible
      Real C_idealVal = cMeasurement.value[i];
      
      //if (measurementType == "Doppler_RangeRate")
      if (measurementType == "RangeRate")
      {         
         // Compute bias
         //ComputeMeasurementBias("Bias", "Doppler_RangeRate", 2);
         ComputeMeasurementBias("Bias", "RangeRate", 2);
         // Compute noise sigma
         //ComputeMeasurementNoiseSigma("NoiseSigma", "Doppler_RangeRate", 2);
         ComputeMeasurementNoiseSigma("NoiseSigma", "RangeRate", 2);
         // Compute measurement error covariance matrix
         ComputeMeasurementErrorCovarianceMatrix();

         // if need range value only, skip this section, otherwise add noise and bias as possible
         if (!rangeOnly)
         {
            // Add noise to measurement value
            if (addNoise)
            {
               // Add noise here
               if (cMeasurement.unfeasibleReason != "R")
               {
                  RandomNumber* rn = RandomNumber::Instance();
                  Real val = rn->Gaussian(cMeasurement.value[i], noiseSigma[i]);
                  cMeasurement.value[i] = val;
               }
            }
         
            //Add bias to measurement value only after noise had been added in order to avoid adding bias' noise 
            if (addBias)
            {
               #ifdef DEBUG_RANGE_CALCULATION
                  MessageInterface::ShowMessage("      . Add bias...\n");
               #endif
               cMeasurement.value[i] = cMeasurement.value[i] + measurementBias[i];
            }
         }
      }


      #ifdef DEBUG_DOPPLER_CALCULATION
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("====  GNDopplerAdapter: Range Calculation for Measurement Data %dth\n", i);
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("      . Measurement epoch             : %.12lf\n", cMeasurement.epochGT.GetMjd());
         MessageInterface::ShowMessage("      . Measurement type              : <%s>\n", measurementType.c_str());
         MessageInterface::ShowMessage("      . Noise adding option           : %s\n", (addNoise?"true":"false"));
         MessageInterface::ShowMessage("      . Doppler count interval        : %.12lf seconds\n", dopplerCountInterval);
         MessageInterface::ShowMessage("      . Real travel lenght for S-path : %.12lf Km\n", dtS);
         MessageInterface::ShowMessage("      . Real travel lenght for E-path : %.12lf Km\n", dtE);
         MessageInterface::ShowMessage("      . Travel length difference dtdt : %.12lf Km\n", dtdt);
         MessageInterface::ShowMessage("      . Turn around ratio             : %.12lf\n", turnaround);
         MessageInterface::ShowMessage("      . Ramp table is %s used\n", (rampTable?"":"not"));
         MessageInterface::ShowMessage("      . Multiplier factor for S-path  : %.12lf\n", multiplierS);
         MessageInterface::ShowMessage("      . Multiplier factor for E-path  : %.12lf\n", multiplierE);
         MessageInterface::ShowMessage("      . C-value w/o noise and bias    : %.12lf Km/s\n", C_idealVal);
         //if (measurementType == "Doppler_RangeRate")
         if (measurementType == "RangeRate")
         {
            MessageInterface::ShowMessage("      . GNDoppler noise sigma      : %.12lf Km/s \n", noiseSigma[i]);
            MessageInterface::ShowMessage("      . GNDoppler bias             : %.12lf Km/s \n", measurementBias[i]);
         }

         MessageInterface::ShowMessage("      . C-value with noise and bias   : %.12lf Km/s\n", cMeasurement.value[i]);
         MessageInterface::ShowMessage("      . Measurement epoch A1Mjd       : %.12lf\n", cMeasurement.epoch); 
         MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible?"feasible":"unfeasible"));
         MessageInterface::ShowMessage("      . Feasibility reason            : %s\n", cMeasurement.unfeasibleReason.c_str());
         MessageInterface::ShowMessage("      . Elevation angle               : %.12lf degree\n", cMeasurement.feasibilityValue);
         MessageInterface::ShowMessage("      . Covariance matrix             : <%p>\n", cMeasurement.covariance);
         if (cMeasurement.covariance)
         {
            MessageInterface::ShowMessage("      . Covariance matrix size = %d\n", cMeasurement.covariance->GetDimension());
            MessageInterface::ShowMessage("     [ ");
            for (UnsignedInt i = 0; i < cMeasurement.covariance->GetDimension(); ++i)
            {
               if ( i > 0)
                  MessageInterface::ShowMessage("\n");
               for (UnsignedInt j = 0; j < cMeasurement.covariance->GetDimension(); ++j)
                  MessageInterface::ShowMessage("%le   ", cMeasurement.covariance->GetCovariance()->GetElement(i,j));
            }
            MessageInterface::ShowMessage("]\n");
         }
         MessageInterface::ShowMessage("===================================================================\n");
      #endif

   }

   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("GNDopplerAdapter::CalculateMeasurement(%s, "
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
const std::vector<RealArray>& GNDopplerAdapter::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id)
{
   if (!calcData)
      throw MeasurementException("Measurement derivative data was requested "
            "for " + instanceName + " before the measurement was set");

   Integer parmId = GetParmIdFromEstID(id, obj);
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Enter GNDopplerAdapter::CalculateMeasurementDerivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n", obj->GetFullName().c_str(), id, parmId, cMeasurement.epoch);
   #endif
   
   // Get parameter name specified by id
   Integer parameterID;
   if (id > 250)
      parameterID = id - obj->GetType() * 250; // GetParmIdFromEstID(id, obj);
   else
      parameterID = id;
   std::string paramName = obj->GetParameterText(parameterID);

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Solve-for parameter: %s\n", paramName.c_str());
   #endif

   // Clear derivative variable
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      theDataDerivatives[i].clear();
   theDataDerivatives.clear();

   if (paramName == "Bias")
   {
      //if (((ErrorModel*)obj)->GetStringParameter("Type") == "Doppler_RangeRate")
      if (((ErrorModel*)obj)->GetStringParameter("Type") == "RangeRate")
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
      // Perform the calculations
      const std::vector<RealArray> *derivativeDataE =
         &(calcData->CalculateMeasurementDerivatives(obj, id));

      const std::vector<RealArray> *derivativeDataS =
         &(adapterS->CalculateMeasurementDerivatives(obj, id));

      #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("   Derivatives E-path: [");
      for (UnsignedInt i = 0; i < derivativeDataE->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataE->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataE->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Haft of Derivatives S-path: [");
      for (UnsignedInt i = 0; i < derivativeDataS->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataS->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataS->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      #endif

      // copy S and E paths' derivatives
      UnsignedInt size = derivativeDataE->at(0).size();
      std::vector<RealArray> derivativesE;
      for (UnsignedInt i = 0; i < derivativeDataE->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesE.push_back(oneRow);

         if (derivativeDataE->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesE[i][j] = (derivativeDataE->at(i))[j];
         }
      }
   
      size = derivativeDataS->at(0).size();
      std::vector<RealArray> derivativesS;
      for (UnsignedInt i = 0; i < derivativeDataS->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesS.push_back(oneRow);

         if (derivativeDataS->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesS[i][j] = (derivativeDataS->at(i))[j] / adapterS->GetMultiplierFactor();       // convert to full range derivatives
         }
      }

      #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("   Derivatives E-path: [");
      for (UnsignedInt i = 0; i < derivativesE.size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativesE[i].size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", derivativesE[i][j]);
         }
      }
      MessageInterface::ShowMessage("] * multiplierE = %.12le\n", multiplierE);
      MessageInterface::ShowMessage("   Derivatives S-path: [");
      for (UnsignedInt i = 0; i < derivativesS.size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativesS[i].size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", derivativesS[i][j]);
         }
      }
      MessageInterface::ShowMessage("] * multiplierS = %.12le\n", multiplierS);
      #endif

      // Now assemble the derivative data into the requested derivative
      size = derivativesE[0].size();

      //theDataDerivatives.clear();
      for (UnsignedInt i = 0; i < derivativesE.size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);

         if (derivativesE[i].size() != size)
            throw MeasurementException("Derivative data size for E path is a different size "
               "than expected");
         if (derivativesS[i].size() != size)
            throw MeasurementException("Derivative data size for S path is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            if ((paramName == "Position")||(paramName == "Velocity")||(paramName == "CartesianX"))
            {
               // Convert measurement derivatives from km/s to Hz for velocity and position 
               theDataDerivatives[i][j] = derivativesE[i][j] * multiplierE - derivativesS[i][j] * multiplierS;
            }
            else
            {
               // set the same E path 's derivatives for Bias an other solve-for variables
               theDataDerivatives[i][j] = derivativesE[i][j];
            }
         }
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
      MessageInterface::ShowMessage("Exit GNDopplerAdapter::CalculateMeasurementDerivatives():\n");
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
bool GNDopplerAdapter::WriteMeasurements()
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
bool GNDopplerAdapter::WriteMeasurement(Integer id)
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
Integer GNDopplerAdapter::HasParameterCovariances(Integer parameterId)
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
Integer GNDopplerAdapter::GetEventCount()
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
void GNDopplerAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   adapterS->SetCorrection(correctionName, correctionType);
   RangeAdapterKm::SetCorrection(correctionName, correctionType);
}

