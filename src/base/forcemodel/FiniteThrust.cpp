//$Header$
//------------------------------------------------------------------------------
//                              FiniteThrust
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Implements the FiniteThrust class used to model the acceleration during a 
 * finite burn. 
 */
//------------------------------------------------------------------------------


#include "FiniteThrust.hpp"

//#define DEBUG_FINITETHRUST_INIT
//#define DEBUG_FINITETHRUST_EXE


#include "MessageInterface.hpp"
#include <sstream>               // for stringstream


//------------------------------------------------------------------------------
// FiniteThrust(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor for forces from finite burns.
 * 
 * @param name Name of the constructed instance.
 */
//------------------------------------------------------------------------------
FiniteThrust::FiniteThrust(const std::string &name) :
   PhysicalModel        (Gmat::PHYSICAL_MODEL, "FiniteThrust", name)
{
}


//------------------------------------------------------------------------------
// ~FiniteThrust()
//------------------------------------------------------------------------------
/**
 * Destructor for forces from finite burns.
 */
//------------------------------------------------------------------------------
FiniteThrust::~FiniteThrust()
{
}


//------------------------------------------------------------------------------
// FiniteThrust(const FiniteThrust& ft)
//------------------------------------------------------------------------------
/**
 * Copy constructor for forces from finite burns.
 * 
 * @param ft The oroginal that gets copied.
 */
//------------------------------------------------------------------------------
FiniteThrust::FiniteThrust(const FiniteThrust& ft) :
   PhysicalModel          (ft),
   burnNames              (ft.burnNames),
   mySpacecraft           (ft.mySpacecraft)
{
        burns.clear();
        scIndices.clear();
}


//------------------------------------------------------------------------------
// FiniteThrust& operator=(const FiniteThrust& ft)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param ft Instance that provides the parameter values.
 * 
 * @return This instance, set to match the input instance.
 */
//------------------------------------------------------------------------------
FiniteThrust& FiniteThrust::operator=(const FiniteThrust& ft)
{
   if (this == &ft)
      return *this;
        
   PhysicalModel::operator=(ft);
   
   burnNames    = ft.burnNames;
   mySpacecraft = ft.mySpacecraft;
   mySpacecraft = ft.mySpacecraft;
   burns.clear();
        scIndices.clear();
   spacecraft->clear();
      
   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * AOverridden Clone method used to make copies from a GmatBase pointer.
 * 
 * @return New instance, set to look like this one.
 */
//------------------------------------------------------------------------------
GmatBase* FiniteThrust::Clone() const
{
   return (new FiniteThrust(*this));
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
void FiniteThrust::Clear(const Gmat::ObjectType type)
{
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACECRAFT)) {
      mySpacecraft.clear();
      spacecraft->clear();
   }
   
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::THRUSTER)) {
      burnNames.clear();
   }
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the names for referenced objects.
 * 
 * FiniteThrust instances use Spacecraft and FiniteThrust objects.  This method
 * sets the names for those objects.
 * 
 * @param type The type of object that has the name.
 * @param name The object's name
 * 
 * @return true on success, false otherwise.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::SetRefObjectName(const Gmat::ObjectType type,
                                  const std::string &name)
{
   if (type == Gmat::SPACECRAFT) {
      if (find(mySpacecraft.begin(), mySpacecraft.end(), name) == mySpacecraft.end())
         mySpacecraft.push_back(name);
      return true;
   }
   
   if (type == Gmat::FINITE_BURN) {
      if (find(burnNames.begin(), burnNames.end(), name) == burnNames.end())
         burnNames.push_back(name);
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
const StringArray& FiniteThrust::GetRefObjectNameArray(
                                                   const Gmat::ObjectType type)
{
   if (type == Gmat::SPACECRAFT)
      return mySpacecraft;
      
   return PhysicalModel::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type, 
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets referenced object pointers.
 * 
 * @param obj The onject.
 * @param type The type of the object.
 * @param name The object's name.
 * 
 * @return true if the object is set, false if not.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::SetRefObject(GmatBase *obj, const Gmat::ObjectType type, 
                                const std::string &name)
{
   if (type == Gmat::FINITE_BURN) {
      if (obj->GetTypeName() != "FiniteBurn")
         throw ForceModelException(
            "FiniteThrust::SetRefObject cannot use objects of type " + 
            obj->GetTypeName());
      if (find(burns.begin(), burns.end(), obj) == burns.end())
         burns.push_back((FiniteBurn*)obj);
      return true;
   }
   
   return PhysicalModel::SetRefObject(obj, type, name);
}

// In case it's needed later
// bool FiniteThrust::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                 const std::string &name, const Integer index)
// {
//    return PhysicalModel::SetRefObject(obj, type, name, index);
// }

//------------------------------------------------------------------------------
// bool IsTransient()
//------------------------------------------------------------------------------
/**
 * Specifies whether the force is transient.
 * 
 * @return true for all FiniteThrust forces.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::IsTransient()
{
   return true;
}


//------------------------------------------------------------------------------
// bool IsTransient()
//------------------------------------------------------------------------------
/**
 * Sets the list of propagated space objects for transient forces.
 */
//------------------------------------------------------------------------------
void FiniteThrust::SetPropList(std::vector<SpaceObject*> *soList)
{
   spacecraft = soList;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Set up data structures to manage finite burns
 * 
 * @return              true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::Initialize()
{
   if (!PhysicalModel::Initialize())
      throw ForceModelException("Unable to initialize FiniteThrust base");
      
   // set up the indices into the state vector that match spacecraft with active 
   // thrusters
   Integer satIndex, stateIndex;
   for (StringArray::iterator satName = mySpacecraft.begin(); 
        satName != mySpacecraft.end(); ++satName)
   {
      satIndex = 0;
      stateIndex = 0;
      
      for (std::vector<SpaceObject *>::iterator propSat = spacecraft->begin();
           propSat != spacecraft->end(); ++propSat) {
         if ((*propSat)->GetName() == *satName) {
            #ifdef DEBUG_FINITETHRUST_INIT
               MessageInterface::ShowMessage(
                  "FiniteThrust::Initialize Matched up %s\n", satName->c_str());
            #endif
         }
         stateIndex += (*propSat)->GetState().GetSize();
         ++satIndex;
      }
   }
   
   return true;
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
 * @param dt            Additional time increment for the derivatitive 
 *                      calculation; defaults to 0.
 * @param state         Pointer to the current state data.  This can differ
 *                      from the PhysicalModel state if the subscribing
 *                      integrator samples other state values during 
 *                      propagation.  (For example, the Runge-Kutta integrators 
 *                      do this during the stage calculations.)
 * @param order         The order of the derivative to be taken (first 
 *                      derivative, second derivative, etc)
 *
 * @return              true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::GetDerivatives(Real * state, Real dt, Integer order)
{
   #ifdef DEBUG_FINITETHRUST_EXE
      MessageInterface::ShowMessage("FiniteThrust::GetDerivatives entered\n");
   #endif

   Real accel[3], mDot, burnData[4];
   Integer i6;
   Integer i = 0;
   SpaceObject *sat;
   Real mTotal;
   
   // Loop through the spacecraft list, building accels for active sats
   for (std::vector<SpaceObject *>::iterator sc = spacecraft->begin();
        sc != spacecraft->end(); ++sc) 
   {
      i6 = i * 6;
      // Just a convenience
      sat = *sc;

      #ifdef DEBUG_FINITETHRUST_EXE
         MessageInterface::ShowMessage("   Checking spacecraft %s\n", 
            sat->GetName().c_str());
      #endif

      if (find(mySpacecraft.begin(), mySpacecraft.end(), sat->GetName()) != 
          mySpacecraft.end()) 
      {

         #ifdef DEBUG_FINITETHRUST_EXE
            MessageInterface::ShowMessage("   Maneuvering ");
         #endif

         if (sat->GetType() != Gmat::SPACECRAFT)
            throw ForceModelException("FiniteThrust::GetDerivatives Finite "
            "burns cannot maneuver " + sat->GetTypeName() + " objects");
         // Start with zero thrust
         mDot = accel[0] = accel[1] = accel[2] = 0.0;

         // Accumulate thrust and mass flow for each active thruster
         for (std::vector <FiniteBurn*>::iterator fb = burns.begin();
              fb != burns.end(); ++fb)
         {
            (*fb)->SetSpacecraftToManeuver((Spacecraft*)sat);
            Real now = epoch + dt / 86400.0;
            if ((*fb)->Fire(burnData, now)) 
            {
               #ifdef DEBUG_FINITETHRUST_EXE
                  MessageInterface::ShowMessage("%s ",(*fb)->GetName().c_str());
               #endif
               accel[0] += burnData[0];
               accel[1] += burnData[1];
               accel[2] += burnData[2];
               mDot     += burnData[3];
            }
            #ifdef DEBUG_FINITETHRUST_EXE
               MessageInterface::ShowMessage(
                  "\n   Acceleration = [%18le  %18le  %18le]", accel[0], 
                  accel[1], accel[2]);
            #endif
         }
   
         #ifdef DEBUG_FINITETHRUST_EXE
            MessageInterface::ShowMessage("\n");
         #endif
         // Divide through by masses to get accelerations
         mTotal = sat->GetRealParameter("TotalMass");
         if (mTotal <= 0.0) 
         {
            std::stringstream massval;
            massval << mTotal;
            throw ForceModelException("Finite thrust applied to spacecraft " +
                     sat->GetName() + " which has nonphysical mass " +
                     massval.str());
         }
         accel[0] /= mTotal;
         accel[1] /= mTotal; 
         accel[2] /= mTotal;
         
         // Apply the burns to the state vector
         if (order == 1) 
         {
            // dr/dt = v
            deriv[i6]     = state[3 + i6];
            deriv[1 + i6] = state[4 + i6];
            deriv[2 + i6] = state[5 + i6];
            deriv[3 + i6] = accel[0];
            deriv[4 + i6] = accel[1];
            deriv[5 + i6] = accel[2];
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
            deriv[i6]     = state[3 + i6];
            deriv[1 + i6] = state[4 + i6];
            deriv[2 + i6] = state[5 + i6];
            deriv[3 + i6] = 0.0;
            deriv[4 + i6] = 0.0;
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

   #ifdef DEBUG_FINITETHRUST_EXE
       //  ShowDerivative("FiniteThrust::GetDerivatives() AFTER compute", state, 
       //                 satCount);
       MessageInterface::ShowMessage("FiniteThrust::GetDerivatives finished\n");
   #endif
   
   return true;
}
