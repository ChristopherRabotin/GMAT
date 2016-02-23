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
   segments                (NULL),
   mDotIndex               (-1),
   depleteMass             (false)
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
   mySpacecraft            (ft.mySpacecraft),
   satCount                (ft.satCount),
   cartIndex               (ft.cartIndex),
   fillCartesian           (ft.fillCartesian),
   segments                (ft.segments),
   mDotIndex               (ft.mDotIndex),
   depleteMass             (ft.depleteMass)
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

      mySpacecraft = ft.mySpacecraft;
      satCount      = ft.satCount;
      cartIndex     = ft.cartIndex;
      fillCartesian = ft.fillCartesian;
      segments      = ft.segments;
      mDotIndex     = ft.mDotIndex;
      depleteMass   = ft.depleteMass;
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
      mySpacecraft.clear();
      spacecraft.clear();
   }
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
      if (find(mySpacecraft.begin(), mySpacecraft.end(), name) == mySpacecraft.end())
         mySpacecraft.push_back(name);
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
   if (type == Gmat::SPACECRAFT)
      return mySpacecraft;

   return PhysicalModel::GetRefObjectNameArray(type);
}

//bool FileThrust::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//      const std::string& name)
//{
//   bool retval = false;
//   return retval;
//}
//
//bool FileThrust::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//      const std::string& name, const Integer index)
//{
//   bool retval = false;
//   return retval;
//}


bool FileThrust::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
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
   MessageInterface::ShowMessage("FileThrust::SetPropList called with %d "
         "objects\n", soList->size());

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
MessageInterface::ShowMessage("Initializing FileThrust\n");

   bool retval = false;

   isInitialized = PhysicalModel::Initialize();

   if (isInitialized)
   {
      if ((segments != NULL) && (mySpacecraft.size() > 0))
      {
         // set up the indices into the state vector that match spacecraft
         Integer satIndex, stateIndex;
         for (StringArray::iterator satName = mySpacecraft.begin();
              satName != mySpacecraft.end(); ++satName)
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
 * @param dt            Additional time increment for the derivatitive
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

   #ifdef DEBUG_FILETHRUST_EXECUTION
      MessageInterface::ShowMessage("Supplying derivative data from a THF\n");
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

         #ifdef DEBUG_FINITETHRUST_EXE
            MessageInterface::ShowMessage("   Checking spacecraft %s\n",
               sat->GetName().c_str());
         #endif

         if (find(mySpacecraft.begin(), mySpacecraft.end(), sat->GetName()) !=
             mySpacecraft.end())
         {

            #ifdef DEBUG_FINITETHRUST_EXE
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

            #ifdef DEBUG_FINITETHRUST_EXE
               MessageInterface::ShowMessage("%s ",(*fb)->GetName().c_str());
            #endif

            // Factor used to convert m/s^2 to km/s^2, and to divide out mass if modeling thrust
            Real factor = 0.001;
            if (dataIsThrust)
            {
               // todo: replace with an int
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
               if (order != 1)
                  throw ODEModelException("Mass depletion cannot be "
                        "performed with the selected propagator.");
               mDot += burnData[3];
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
                  deriv[mloc+i]   = mDot;  // Is this right???
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
            // Zero any burn contribution that is not made for this spacecraft
            if (order == 1)
            {
               // dr/dt = v
                  deriv[i6]     =
                  deriv[1 + i6] =
                  deriv[2 + i6] =
                  deriv[3 + i6] =
                  deriv[4 + i6] =
               deriv[5 + i6] = 0.0;
            }
            else
            {
               deriv[ i6 ] = 0.0;
               deriv[i6+1] = 0.0;
               deriv[i6+2] = 0.0;
               deriv[i6+3] = 0.0;
               deriv[i6+4] = 0.0;
               deriv[i6+5] = 0.0;
            }
         }
         ++i;
      }
   }

   #ifdef DEBUG_FINITETHRUST_EXE
      //ShowDerivative("FiniteThrust::GetDerivatives() AFTER compute", state,
      //               satCount);
      MessageInterface::ShowMessage
         ("     deriv[1:3] = [%18le %18le %18le]\n     deriv[4:6] = [%18le %18le %18le]\n",
          deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5]);
      MessageInterface::ShowMessage("FiniteThrust::GetDerivatives finished\n");
   #endif

   return retval;
}

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
         break;
      }
   }

   if (index != -1)
   {
      Real offset = atEpoch - (*segments)[index].segData.startEpoch;

      if ((*segments)[index].segData.accelIntType == ThfDataSegment::NONE)
      {
         // If at the end point; use its data
         if ((*segments)[index].segData.endEpoch == atEpoch)
         {
            Integer i = (*segments)[index].segData.profile.size()-1;
            burnData[0] = (*segments)[index].segData.profile[i].vector[0];
            burnData[1] = (*segments)[index].segData.profile[i].vector[1];
            burnData[2] = (*segments)[index].segData.profile[i].vector[2];
         }
         else
         {
            for (UnsignedInt i = 0; i < (*segments)[index].segData.profile.size()-1; ++i)
            {
               if (((*segments)[index].segData.profile[i].time <= offset) &&
                   ((*segments)[index].segData.profile[i+1].time > offset))
               {
                  burnData[0] = (*segments)[index].segData.profile[i].vector[0];
                  burnData[1] = (*segments)[index].segData.profile[i].vector[1];
                  burnData[2] = (*segments)[index].segData.profile[i].vector[2];
               }
            }
         }
      }
      else throw ODEModelException("The " +
            (*segments)[index].segData.interpolationMethod +
            " interpolation method is not yet implemented");
   }
}
