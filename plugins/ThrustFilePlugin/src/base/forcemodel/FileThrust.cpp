//------------------------------------------------------------------------------
//                           FileThrust
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "FileThrust.hpp"

#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "ODEModelException.hpp"
#include <sstream>               // for stringstream


//#define DEBUG_FILETHRUST_EXE
//#define DEBUG_MASS_FLOW
//#define DEBUG_SEGMENTS
//#define DEBUG_REF_OBJECTS

//------------------------------------------------------------------------------
// FileThrust(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor for forces from thrust history files.
 *
 * @param name Name of the constructed instance.
 */
//------------------------------------------------------------------------------
FileThrust::FileThrust(const std::string &name) :
   PhysicalModel           (Gmat::PHYSICAL_MODEL, "FileThrust", name),
   satCount                (0),
   cartIndex               (-1),
   fillCartesian           (true),
   dataIsThrust            (true),
   massFlowWarningNeeded   (true),
   segments                (NULL),
   mDotIndex               (-1),
   depleteMass             (false),
   coordSystem             (NULL),
   liner                   (NULL),
   spliner                 (NULL)
{
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   objectTypeNames.push_back("FileThrust");
}

//------------------------------------------------------------------------------
// ~FileThrust()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FileThrust::~FileThrust()
{
   if (liner != NULL)
      delete liner;
   if (spliner != NULL)
      delete spliner;
}

//------------------------------------------------------------------------------
// FileThrust(const FileThrust& ft)
//------------------------------------------------------------------------------
/**
 * Copy constructor for forces from thrust history files.
 *
 * @param ft The original that gets copied.
 */
//------------------------------------------------------------------------------
FileThrust::FileThrust(const FileThrust& ft) :
   PhysicalModel           (ft),
   spacecraftNames         (ft.spacecraftNames),
   satCount                (ft.satCount),
   cartIndex               (ft.cartIndex),
   fillCartesian           (ft.fillCartesian),
   dataIsThrust            (ft.dataIsThrust),
   massFlowWarningNeeded   (true),
   segments                (ft.segments),
   mDotIndex               (ft.mDotIndex),
   depleteMass             (ft.depleteMass),
   activeTankName          (ft.activeTankName),
   csNames                 (ft.csNames),
   coordSystem             (NULL),
   liner                   (NULL),
   spliner                 (NULL)
{
}

//------------------------------------------------------------------------------
// FileThrust& operator=(const FileThrust& ft)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param ft Instance that provides the parameter values.
 *
 * @return This instance, set to match the input instance.
 */
//------------------------------------------------------------------------------
FileThrust& FileThrust::operator=(const FileThrust& ft)
{
   if (this != &ft)
   {
      PhysicalModel::operator=(ft);

      spacecraftNames = ft.spacecraftNames;
      satCount      = ft.satCount;
      cartIndex     = ft.cartIndex;
      fillCartesian = ft.fillCartesian;
      dataIsThrust  = ft.dataIsThrust;
      segments      = ft.segments;
      mDotIndex     = ft.mDotIndex;
      depleteMass   = ft.depleteMass;
      activeTankName = ft.activeTankName;
      csNames       = ft.csNames;
      coordSystem   = NULL;
      if (liner != NULL)
      {
         delete liner;
         liner = NULL;
      }
      if (spliner != NULL)
      {
         delete spliner;
         spliner = NULL;
      }

      massFlowWarningNeeded = true;
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool operator==(const FileThrust& ft) const
//------------------------------------------------------------------------------
/**
 * Checks to see if two FileThrusts apply the same force.
 *
 * @param ft The second FileThrust
 *
 * @return true if the segment data are the same.
 */
//------------------------------------------------------------------------------
bool FileThrust::operator==(const FileThrust& ft) const
{
   return (segments == ft.segments);
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Overridden Clone method used to make copies from a GmatBase pointer.
 *
 * @return New instance, set to look like this one.
 */
//------------------------------------------------------------------------------
GmatBase* FileThrust::Clone() const
{
   return new FileThrust(*this);
}

//------------------------------------------------------------------------------
// void Clear(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Clears the arrays of elements that get set by the Propagate commands
 *
 * @param type The type of element that gets cleared.  Set to
 *             Gmat::UNKNOWN_OBJECT to clear all of the configrable arrays.
 */
//------------------------------------------------------------------------------
void FileThrust::Clear(const Gmat::ObjectType type)
{
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACECRAFT))
   {
      spacecraftNames.clear();
      spacecraft.clear();
   }
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 *
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& FileThrust::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   refObjectTypes.push_back(Gmat::SPACECRAFT);
   return refObjectTypes;
}

//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets the names for referenced objects.
 *
 * FileThrust instances use Spacecraft and (set separately) thrust segment
 * objects.  This method sets the names for the Spacecraft objects.
 *
 * @param type The type of object that has the name.
 * @param name The object's name
 *
 * @return true on success, false otherwise.
 */
//------------------------------------------------------------------------------
bool FileThrust::SetRefObjectName(const Gmat::ObjectType type,
      const std::string& name)
{
   if (type == Gmat::SPACECRAFT)
   {
      if (find(spacecraftNames.begin(), spacecraftNames.end(), name) == spacecraftNames.end())
         spacecraftNames.push_back(name);
      return true;
   }

   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (find(csNames.begin(), csNames.end(), name) == csNames.end())
         csNames.push_back(name);
      return true;
   }

   return PhysicalModel::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Accesses the names for referenced objects.
 *
 * @param type The type of object requested.
 *
 * @return The vector of names.
 */
//------------------------------------------------------------------------------
const StringArray& FileThrust::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   #ifdef DEBUG_REF_OBJECTS
      MessageInterface::ShowMessage("FileThrust::GetRefObjectNameArray called "
            "for type %d\n", type);
   #endif
   PhysicalModel::GetRefObjectNameArray(type);

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::UNKNOWN_OBJECT))
      refObjectNames.insert(refObjectNames.begin(), spacecraftNames.begin(),
            spacecraftNames.end());

   if ((type == Gmat::COORDINATE_SYSTEM) || (type == Gmat::UNKNOWN_OBJECT))
      refObjectNames.insert(refObjectNames.begin(), csNames.begin(),
            csNames.end());

   #ifdef DEBUG_REF_OBJECTS
      MessageInterface::ShowMessage("  Returning %d names\n",
            refObjectNames.size());
   #endif

   return refObjectNames;
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets referenced object pointers.
 *
 * @param obj The object.
 * @param type The type of the object.
 * @param name The object's name.
 *
 * @return true if the object is set, false if not.
 */
//------------------------------------------------------------------------------
bool FileThrust::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
      const std::string& name)
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      #ifdef DEBUG_REF_OBJECTS
         MessageInterface::ShowMessage("Setting cs named %s\n", name.c_str());
      #endif

      bool retval = false;

      if (obj->IsOfType("CoordinateSystem") == false)
         throw ODEModelException(
            "FileThrust::SetRefObject cannot use objects of type " +
            obj->GetTypeName());
      if (find(csNames.begin(), csNames.end(), name) != csNames.end())
      {
         // Pass the CS to each segment that needs it
         for (UnsignedInt i = 0; i < segments->size(); ++i)
         {
            if (segments->at(i).segData.csName == name)
            {
               segments->at(i).segData.cs = (CoordinateSystem*)obj;
               retval = true;
            }
         }
      }
      return retval;
   }

   return PhysicalModel::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets referenced object pointers in an object array.
 *
 * @param obj The object.
 * @param type The type of the object.
 * @param name The object's name.
 * @param index The index of the object in the array
 *
 * @return true if the object is set, false if not.
 */
//------------------------------------------------------------------------------
bool FileThrust::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
      const std::string& name, const Integer index)
{
   MessageInterface::ShowMessage("Setting %s\n", name.c_str());
   return PhysicalModel::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string &oldName,
//       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Changes the name for reference objects
 *
 * @param type The type of the object.
 * @param oldName The object's name before the change.
 * @param newName The object's proposed name after the change.
 *
 * @return true if a change was made, false if not
 */
//------------------------------------------------------------------------------
bool FileThrust::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   if (type == Gmat::SPACECRAFT)
   {
      for (UnsignedInt i = 0; i < spacecraftNames.size(); ++i)
      {
         if (spacecraftNames[i] == oldName)
            spacecraftNames[i] = newName;
      }
      return true;
   }

   if (type == Gmat::COORDINATE_SYSTEM)
   {
      for (UnsignedInt i = 0; i < csNames.size(); ++i)
      {
         if (csNames[i] == oldName)
            csNames[i] = newName;
      }
      return true;
   }

   return PhysicalModel::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a reference object
 *
 * @param type The type of the object
 * @param name The object's name
 *
 * @return The object pointer
 */
//------------------------------------------------------------------------------
GmatBase* FileThrust::GetRefObject(const Gmat::ObjectType type,
      const std::string& name)
{
   if (type == Gmat::SPACECRAFT)
   {
      if (name == "")
         if (spacecraft.size() > 0)
            return spacecraft[0];
      for (UnsignedInt i = 0; i < spacecraft.size(); ++i)
         if (spacecraft[i]->GetName() == name)
            return spacecraft[i];
      return NULL;
   }

   return PhysicalModel::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a reference object from an array
 *
 * @param type The type of the object
 * @param name The object's name
 * @param index The index of the object in the array
 *
 * @return The object pointer
 */
//------------------------------------------------------------------------------
GmatBase* FileThrust::GetRefObject(const Gmat::ObjectType type,
      const std::string& name, const Integer index)
{
   return PhysicalModel::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool IsTransient()
//------------------------------------------------------------------------------
/**
 * Specifies whether the force is transient.
 *
 * @return true for all FiniteThrust forces.
 */
//------------------------------------------------------------------------------
bool FileThrust::IsTransient()
{
   return true;
}

//------------------------------------------------------------------------------
// bool DepletesMass()
//------------------------------------------------------------------------------
/**
 * Detects mass depletion from a PhysicalModel
 *
 * @return true if the model depletes mass, false if it does not
 */
//------------------------------------------------------------------------------
bool FileThrust::DepletesMass()
{
   return depleteMass;
}

//------------------------------------------------------------------------------
// void FileThrust::SetSegmentList(std::vector<ThrustSegment> *segs)
//------------------------------------------------------------------------------
/**
 * Sets the list of thrust file segments used by the force
 *
 * @param segs The segment list
 */
//------------------------------------------------------------------------------
void FileThrust::SetSegmentList(std::vector<ThrustSegment> *segs)
{
   segments = segs;
   depleteMass = false;

   // Activate mass flow if any segment needs it and collect ref objects
   csNames.clear();
   for (UnsignedInt i = 0; i < segments->size(); ++i)
   {
      depleteMass |= segments->at(i).DepletesMass();

      // Collect the names of all of the coordinate systems needed
      std::string name = segments->at(i).segData.csName;
      if (find(csNames.begin(), csNames.end(), name) == csNames.end())
         csNames.push_back(name);
   }

   #ifdef DEBUG_SEGMENTS
      // Dump seg list here
      MessageInterface::ShowMessage("%d segments in the list\n", segments->size());
      for (UnsignedInt i = 0; i < segments->size(); ++i)
      {
         MessageInterface::ShowMessage("%d:  %s\n", i,
               segments->at(i).GetName().c_str());
         MessageInterface::ShowMessage("     TSF:   %lf\n",
               segments->at(i).GetRealParameter("ThrustScaleFactor"));
         MessageInterface::ShowMessage("     MFSF:  %lf\n",
               segments->at(i).GetRealParameter("MassFlowScaleFactor"));
         MessageInterface::ShowMessage("     Tanks: %d\n",
               segments->at(i).GetStringArrayParameter("MassSource").size());
         MessageInterface::ShowMessage("     CS: %s\n",
               segments->at(i).segData.csName.c_str());
      }
   #endif
}

//------------------------------------------------------------------------------
// void SetPropList(ObjectArray *soList)
//------------------------------------------------------------------------------
/**
 * Sets the list of propagated space objects for transient forces.
 */
//------------------------------------------------------------------------------
void FileThrust::SetPropList(ObjectArray *soList)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("FileThrust::SetPropList called with %d "
            "objects\n", soList->size());
   #endif
   spacecraft.clear();
   for (ObjectArray::iterator i = soList->begin(); i != soList->end(); ++i)
      spacecraft.push_back(*i);
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Set up data structures to manage the file based burns
 *
 * @return true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool FileThrust::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Initializing FileThrust\n");
   #endif

   bool retval = false;

   isInitialized = PhysicalModel::Initialize();

   // Zero the data containers
   for (Integer i = 0; i < 5; ++i)
   {
      dataBlock[i] = 0.0;
      dataSet[i][0] = dataSet[i][1] = dataSet[i][2] =
      dataSet[i][3] = dataSet[i][4] = 0.0;
   }

   if (isInitialized)
   {
      if ((segments != NULL) && (spacecraftNames.size() > 0))
      {
         // set up the indices into the state vector that match spacecraft
         Integer satIndex, stateIndex;
         for (StringArray::iterator satName = spacecraftNames.begin();
              satName != spacecraftNames.end(); ++satName)
         {
            satIndex = 0;
            stateIndex = 0;

            for (ObjectArray::iterator propSat = spacecraft.begin();
                 propSat != spacecraft.end(); ++propSat)
            {
               if ((*propSat)->IsOfType(Gmat::SPACEOBJECT))
               {
                  stateIndex += ((SpaceObject*)(*propSat))->GetState().GetSize();
                  ++satIndex;
               }
            }
         }

         massFlowWarningNeeded = true;
         retval = true;
      }
      else
         isInitialized = false;
   }

   if (!isInitialized)
      throw ODEModelException("Unable to initialize FileThrust base");

   return retval;
}

//------------------------------------------------------------------------------
// bool GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Method invoked to calculate derivatives
 *
 * This method is invoked to fill the deriv array with derivative information
 * for the system that is being integrated.  It uses the state and elapsedTime
 * parameters, along with the time interval dt passed in as a parameter, to
 * calculate the derivative information at time \f$t=t_0+t_{elapsed}+dt\f$.
 *
 * @param state         Pointer to the current state data.  This can differ
 *                      from the PhysicalModel state if the subscribing
 *                      integrator samples other state values during
 *                      propagation.  (For example, the Runge-Kutta integrators
 *                      do this during the stage calculations.)
 * @param dt            Additional time increment for the derivative
 *                      calculation; defaults to 0.
 * @param order         The order of the derivative to be taken (first
 *                      derivative, second derivative, etc)
 *
 * @return              true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool FileThrust::GetDerivatives(Real* state, Real dt, Integer order,
      const Integer id)
{
   bool retval = true;

   #ifdef DEBUG_FILETHRUST_EXE
      MessageInterface::ShowMessage("Supplying derivative data from a THF, %s\n",
            (depleteMass ? "including mass flow" : "without mass flow"));
   #endif

   if (fillCartesian)
   {
      Real accel[3], mDot, burnData[4];
      Integer i6, mloc = -1;
      Integer i = 0, j = 0;
      SpaceObject *sat;

      // Loop through the spacecraft list, building accels for active sats
      for (ObjectArray::iterator sc = spacecraft.begin();
           sc != spacecraft.end(); ++sc)
      {
         i6 = cartIndex + i * 6;

         if (1)  // <-- NEED A TEST HERE!
         {
            if (depleteMass)
            {
               mloc = mDotIndex + j;
               ++j;
            }
         }
         else
            mloc = -1;

         if ((*sc)->IsOfType(Gmat::SPACEOBJECT) == false)
            continue;

         // Just a convenience
         sat = (SpaceObject*)(*sc);

         #ifdef DEBUG_FILETHRUST_EXE
            MessageInterface::ShowMessage("   Checking spacecraft %s\n",
               sat->GetName().c_str());
         #endif

         if (find(spacecraftNames.begin(), spacecraftNames.end(), sat->GetName()) !=
             spacecraftNames.end())
         {

            #ifdef DEBUG_FILETHRUST_EXE
               MessageInterface::ShowMessage("   Maneuvering %s\n",
                     (*sc)->GetName().c_str());
            #endif

            if (sat->GetType() != Gmat::SPACECRAFT)
               throw ODEModelException("FiniteThrust::GetDerivatives Finite "
               "burns cannot maneuver " + sat->GetTypeName() + " objects");

            // Start with zero thrust
            mDot = accel[0] = accel[1] = accel[2] = 0.0;
            Real now = epoch + dt / GmatTimeConstants::SECS_PER_DAY;
            ComputeAccelerationMassFlow(now, burnData);

             // Factor used to convert m/s^2 to km/s^2, and to divide out mass if modeling thrust
            Real factor = 0.001;
            if (dataIsThrust)
            {
               // todo: replace string label with an int ID
               Real mass = sat->GetRealParameter("TotalMass");
               factor /= mass;
            }

            accel[0] += burnData[0] * factor;
            accel[1] += burnData[1] * factor;
            accel[2] += burnData[2] * factor;

            #ifdef DEBUG_MASS_FLOW
               MessageInterface::ShowMessage(
                     "   --> direction = [%.12le %.12le %.12le]",
                     accel[0], accel[1], accel[2]);
            #endif

            if (depleteMass)
            {
               // Set the mass flow flag on sat's tanks
               FuelTank *tank = (FuelTank*)(sat->GetRefObject(
                     Gmat::FUEL_TANK, activeTankName));
               if (tank != NULL)
                  tank->SetFlowWithoutThruster(true);

               if (order != 1)
                  throw ODEModelException("Mass depletion cannot be "
                        "performed with the selected propagator.");
               mDot -= burnData[3];
               #ifdef DEBUG_MASS_FLOW
                  MessageInterface::ShowMessage("  mDot =  %.12lf\n",
                        mDot);
               #endif
            }
            #ifdef DEBUG_MASS_FLOW
            else
               MessageInterface::ShowMessage("  mDot =  0.0\n");
            #endif

            // Apply the burns to the state vector
            if (order == 1)
            {
               // dr/dt = v
               deriv[i6]     =
               deriv[1 + i6] =
               deriv[2 + i6] = 0.0;
               deriv[3 + i6] = accel[0];
               deriv[4 + i6] = accel[1];
               deriv[5 + i6] = accel[2];

               if (mloc >= 0)
               {
                  #ifdef DEBUG_MASS_FLOW
                     MessageInterface::ShowMessage("Setting mdot in deriv[%d] "
                           "to %le\n", mloc+i, mDot);
                  #endif
                  deriv[mloc+i] = mDot;
               }
            }
            else
            {
               deriv[ i6 ] = accel[0];
               deriv[i6+1] = accel[1];
               deriv[i6+2] = accel[2];
               deriv[i6+3] = 0.0;
               deriv[i6+4] = 0.0;
               deriv[i6+5] = 0.0;
            }
         }
         else
         {
            deriv[ i6 ] =
            deriv[i6+1] =
            deriv[i6+2] =
            deriv[i6+3] =
            deriv[i6+4] =
            deriv[i6+5] = 0.0;
         }
         ++i;
      }
   }

   #ifdef DEBUG_FILETHRUST_EXE
      //ShowDerivative("FiniteThrust::GetDerivatives() AFTER compute", state,
      //               satCount);
      MessageInterface::ShowMessage
         ("     deriv[1:3] = [%18le %18le %18le]\n"
          "     deriv[4:6] = [%18le %18le %18le]\n"
          "     mdot       = %18le\n",
          deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5],
          (depleteMass ? deriv[mDotIndex] : 0.0));
      MessageInterface::ShowMessage("FiniteThrust::GetDerivatives finished\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft* sc)
//------------------------------------------------------------------------------
/**
 * Retrieves derivative data used in the force model Parameter code
 *
 * @param sc The spacecraft that supplies the needed state data
 *
 * @return The derivative data as a 6-vector
 *
 * @note: Not yet implemented for the Thrust History File reader code.
 */
//------------------------------------------------------------------------------
Rvector6 FileThrust::GetDerivativesForSpacecraft(Spacecraft* sc)
{
   Rvector6 dv;
   return dv;
}

//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative
 * information for a specified type.
 *
 * @param id State Element ID for the derivative type
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool FileThrust::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "FileThrust checking for support for id %d\n", id);
   #endif

   if (id == Gmat::CARTESIAN_STATE)
      return true;

   if (id == Gmat::MASS_FLOW)
   {
      return true;
   }

   return PhysicalModel::SupportsDerivative(id);
}

//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state
 * vector, so that the derivative information can be placed in the correct place
 * in the derivative vector.
 *
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool FileThrust::SetStart(Gmat::StateElementId id, Integer index,
      Integer quantity, Integer sizeOfType)
{
   bool retval = false;

   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartIndex = index;
         fillCartesian = true;
         retval = true;
         break;

      case Gmat::MASS_FLOW:
         // todo: add mass flow bits here
//         satThrustCount = quantity;
         mDotIndex = index;
         depleteMass = true;
         retval = true;
         break;

      default:
         break;
   }

   return retval;
}

//------------------------------------------------------------------------------
// void ComputeAccelerationMassFlow(const GmatEpoch atEpoch, Real burnData[4],
//       SpaceObject *sat)
//------------------------------------------------------------------------------
/**
 * Retrieves dv data at a specified epoch from data in the thrust history file
 *
 * @param atEpoch The epoch of the data request
 * @param burnData The container receiving the acceleration and mass flow
 */
//------------------------------------------------------------------------------
void FileThrust::ComputeAccelerationMassFlow(const GmatEpoch atEpoch,
      Real burnData[4])
{
   // Start from nothin'
   burnData[0] = burnData[1] = burnData[2] = burnData[3] = 0.0;

   Real scaleFactors[2];

   // Find the segment with data covering the input epoch.  Note that if
   // segments overlap, we use the data in the first segment covering the epoch
   Integer index = -1;
   for (UnsignedInt i = 0; i < segments->size(); ++i)
   {
      if (((*segments)[i].segData.startEpoch <= atEpoch) &&
            ((*segments)[i].segData.endEpoch >= atEpoch))
      {
         index = i;
         // Factor used to convert m/s^2 to km/s^2, and to divide out mass if modeling thrust
         dataIsThrust = (*segments)[i].segData.modelThrust;
         (*segments)[i].GetScaleFactors(scaleFactors);
         coordSystem = (*segments)[i].segData.cs;
         break;
      }
   }

   if (index != -1)
   {
//      if ((*segments)[index].segData.profile.size() < 2)
//         throw ODEModelException("Cannot model thrust: The thrust profile does "
//               "not contain enough data");

      // Interpolate the thrust/acceleration
      switch ((*segments)[index].segData.accelIntType)
      {
      case ThfDataSegment::LINEAR:
         LinearInterpolate(index, atEpoch);
         burnData[0] = dataBlock[0] * scaleFactors[0];
         burnData[1] = dataBlock[1] * scaleFactors[0];
         burnData[2] = dataBlock[2] * scaleFactors[0];
         break;

      case ThfDataSegment::SPLINE:
         SplineInterpolate(index, atEpoch);
         burnData[0] = dataBlock[0] * scaleFactors[0];
         burnData[1] = dataBlock[1] * scaleFactors[0];
         burnData[2] = dataBlock[2] * scaleFactors[0];
         break;

      case ThfDataSegment::NONE:
      default:
         GetSegmentData(index, atEpoch);
         burnData[0] = dataBlock[0] * scaleFactors[0];
         burnData[1] = dataBlock[1] * scaleFactors[0];
         burnData[2] = dataBlock[2] * scaleFactors[0];
         break;
      }

      if ((*segments)[index].segData.massIntType == (Integer)dataBlock[4])
      {
         burnData[3] = dataBlock[3] * scaleFactors[1];
      }
      else
      {
         // Interpolate the mass flow, usind data already collected
         switch ((*segments)[index].segData.massIntType)
         {
         case ThfDataSegment::LINEAR:
            LinearInterpolate(index, atEpoch);
            burnData[3] = dataBlock[3] * scaleFactors[1];
            break;

         case ThfDataSegment::SPLINE:
            SplineInterpolate(index, atEpoch);
            burnData[3] = dataBlock[3] * scaleFactors[1];
            break;

         case ThfDataSegment::NONE:
         default:
            GetSegmentData(index, atEpoch);
            burnData[3] = dataBlock[3] * scaleFactors[1];
            break;
         }
      }

      if (burnData[3] != 0.0)
      {
         #ifdef DEBUG_MASS_FLOW
            MessageInterface::ShowMessage("Accessing segment %d at epoch "
                  "%.12lf; [3] = %le\n", index, atEpoch, burnData[3]);
            MessageInterface::ShowMessage("   %d mass sources\n",
                  (*segments)[index].massSource.size());
         #endif
         if (segments->size() > index)
         {
            if ((*segments)[index].massSource.size() > 0)
               activeTankName = (*segments)[index].massSource[0];
            else
            {
               if (massFlowWarningNeeded)
               {
                  MessageInterface::ShowMessage("Warning: The Thrust History "
                        "File force %s cannot deplete mass: no mass source "
                        "is identified\n", instanceName.c_str());
                  massFlowWarningNeeded = false;
               }
               burnData[3] = 0.0;
               depleteMass = false;
               massFlowWarningNeeded = false;
            }
         }
      }

      if (coordSystem != NULL)
         ConvertDirectionToInertial(burnData, burnData, atEpoch);
   }
}


//------------------------------------------------------------------------------
// void GetSegmentData(Integer atIndex, GmatEpoch atEpoch)
//------------------------------------------------------------------------------
/**
 * Retrieves the segment data for the segment containing the input epoch
 *
 * @param atIndex Index of the segment containing the data
 * @param atEpoch The epoch of the requested data
 */
//------------------------------------------------------------------------------
void FileThrust::GetSegmentData(Integer atIndex, GmatEpoch atEpoch)
{
   // If at the end point; use its data
   if ((*segments)[atIndex].segData.endEpoch == atEpoch)
   {
      Integer i = (*segments)[atIndex].segData.profile.size()-1;
      dataBlock[0] = (*segments)[atIndex].segData.profile[i].vector[0];
      dataBlock[1] = (*segments)[atIndex].segData.profile[i].vector[1];
      dataBlock[2] = (*segments)[atIndex].segData.profile[i].vector[2];
      dataBlock[3] = (*segments)[atIndex].segData.profile[i].mdot;
      dataBlock[4] = (*segments)[atIndex].segData.profile[i].time;
   }
   else
   {
      Real offset = atEpoch - (*segments)[atIndex].segData.startEpoch;
      for (UnsignedInt i = 0; i < (*segments)[atIndex].segData.profile.size()-1; ++i)
      {
         if (((*segments)[atIndex].segData.profile[i].time <= offset) &&
             ((*segments)[atIndex].segData.profile[i+1].time > offset))
         {
            dataBlock[0] = (*segments)[atIndex].segData.profile[i].vector[0];
            dataBlock[1] = (*segments)[atIndex].segData.profile[i].vector[1];
            dataBlock[2] = (*segments)[atIndex].segData.profile[i].vector[2];
            dataBlock[3] = (*segments)[atIndex].segData.profile[i].mdot;
            dataBlock[4] = ThfDataSegment::NONE;
         }
      }
   }
}


//------------------------------------------------------------------------------
// void LinearInterpolate(Integer atIndex, GmatEpoch atEpoch)
//------------------------------------------------------------------------------
/**
 * Retrieves linearly interpolated segment data for the input epoch
 *
 * @param atIndex Index of the segment containing the data
 * @param atEpoch The epoch of the requested data
 */
//------------------------------------------------------------------------------
void FileThrust::LinearInterpolate(Integer atIndex, GmatEpoch atEpoch)
{
   GetSegmentData(atIndex, atEpoch);

   if ((*segments)[atIndex].segData.endEpoch == atEpoch)
   {
      return;
   }

   Real offset = atEpoch - (*segments)[atIndex].segData.startEpoch;
   for (UnsignedInt i = 0; i < (*segments)[atIndex].segData.profile.size()-1; ++i)
   {
      if (((*segments)[atIndex].segData.profile[i].time <= offset) &&
          ((*segments)[atIndex].segData.profile[i+1].time > offset))
      {
         dataSet[0][0] = dataBlock[0];
         dataSet[0][1] = dataBlock[1];
         dataSet[0][2] = dataBlock[2];
         dataSet[0][3] = dataBlock[3];
         dataSet[0][4] = (*segments)[atIndex].segData.profile[i].time;

         dataSet[1][0] = (*segments)[atIndex].segData.profile[i+1].vector[0];
         dataSet[1][1] = (*segments)[atIndex].segData.profile[i+1].vector[1];
         dataSet[1][2] = (*segments)[atIndex].segData.profile[i+1].vector[2];
         dataSet[1][3] = (*segments)[atIndex].segData.profile[i+1].mdot;
         dataSet[1][4] = (*segments)[atIndex].segData.profile[i+1].time;
      }
   }

   Real pct = (offset - dataSet[0][4]) / (dataSet[1][4] - dataSet[0][4]);

   dataBlock[0] = dataSet[0][0] + pct * (dataSet[1][0] - dataSet[0][0]);
   dataBlock[1] = dataSet[0][1] + pct * (dataSet[1][1] - dataSet[0][1]);
   dataBlock[2] = dataSet[0][2] + pct * (dataSet[1][2] - dataSet[0][2]);
   dataBlock[3] = dataSet[0][3] + pct * (dataSet[1][3] - dataSet[0][3]);
   dataBlock[4] = ThfDataSegment::LINEAR;

   #ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("Linear Interpolating to offset %.12lf "
            "using t, vector, mdot:\n", offset);
      MessageInterface::ShowMessage("   %.12lf [%.12le  %.12le  %.12le] "
            "%.12le\n   %.12lf [%.12le  %.12le  %.12le] %.12le\n",
            dataSet[0][4], dataSet[0][0], dataSet[0][1], dataSet[0][2],
            dataSet[0][3], dataSet[1][4], dataSet[1][0], dataSet[1][1],
            dataSet[1][2], dataSet[1][3]);

      MessageInterface::ShowMessage("-> %.12lf [%.12le  %.12le  %.12le] "
            "%.12le\n", offset, dataBlock[0], dataBlock[1], dataBlock[2], dataBlock[3]);
   #endif
}

//------------------------------------------------------------------------------
// void SplineInterpolate(Integer atIndex, GmatEpoch atEpoch)
//------------------------------------------------------------------------------
/**
 * Retrieves spline interpolated segment data for the input epoch
 *
 * @param atIndex Index of the segment containing the data
 * @param atEpoch The epoch of the requested data
 */
//------------------------------------------------------------------------------
void FileThrust::SplineInterpolate(Integer atIndex, GmatEpoch atEpoch)
{
   if (spliner == NULL)
      spliner = new NotAKnotInterpolator("SplineInterpolator", 4);

   throw ODEModelException("The cubic spline interpolation method is not yet "
         "implemented");
}


//------------------------------------------------------------------------------
// void ConvertDirectionToInertial(Real *dir, Real *dirInertial, Real epoch)
//------------------------------------------------------------------------------
/*
 * Converts thrust direction to inertial frame
 *
 * @param dir  Thrust/accel direction in internal frame
 * @param dirInertial  Thrust direction in inertial frame
 * @param epoch  Epoch to be used for conversion
 */
//------------------------------------------------------------------------------
void FileThrust::ConvertDirectionToInertial(Real *dir, Real *dirInertial, Real epoch)
{
   #ifdef DEBUG_FILETHRUST_EXE
      MessageInterface::ShowMessage("Rotating into a new frame\n");
   #endif

   Real inDir[6], outDir[6];
   for (Integer i=0; i<3; i++)
      inDir[i] = dir[i];
   for (Integer i=3; i<6; i++)
      inDir[i] = 0.0;

   // Now rotate to base system axes, we don't want to translate so
   // set coincident to true
   coordSystem->ToBaseSystem(epoch, inDir, outDir, true);

   dirInertial[0] = outDir[0];
   dirInertial[1] = outDir[1];
   dirInertial[2] = outDir[2];
}
