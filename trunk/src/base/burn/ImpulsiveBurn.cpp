//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the ImpulsiveBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#include "ImpulsiveBurn.hpp"
#include "MessageInterface.hpp"
 

//#define DEBUG_IMPULSIVE_BURN


//------------------------------------------------------------------------------
//  Burn(const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the impulsive burn (default constructor).
 * 
 * @param <nomme> Name for the object
 */
//------------------------------------------------------------------------------
ImpulsiveBurn::ImpulsiveBurn(const std::string &nomme) :
    Burn(Gmat::IMPULSIVE_BURN, "ImpulsiveBurn", nomme)
{
   objectTypes.push_back(Gmat::IMPULSIVE_BURN);
   objectTypeNames.push_back("ImpulsiveBurn");
}


//------------------------------------------------------------------------------
//  ImpulsiveBurn()
//------------------------------------------------------------------------------
/**
 * Destroys the impulsive burn (destructor).
 */
//------------------------------------------------------------------------------
ImpulsiveBurn::~ImpulsiveBurn()
{
}

//------------------------------------------------------------------------------
//  ImpulsiveBurn(const ImpulsiveBurn &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the impulsive burn from the input burn (copy constructor).
 *
 * @param <copy> ImpulsiveBurn object to copy
 */
//------------------------------------------------------------------------------
ImpulsiveBurn::ImpulsiveBurn(const ImpulsiveBurn &copy) :
Burn            (copy)
{
}

//------------------------------------------------------------------------------
//  ImpulsiveBurn& operator=(const ImpulsiveBurn &orig)
//------------------------------------------------------------------------------
/**
 * Assignment operator for impulsive burns.
 *
 * @param <orig> ImpulsiveBurn object to copy
 *
 * @return this instance, configured like the imput instance.
 */
//------------------------------------------------------------------------------
ImpulsiveBurn& ImpulsiveBurn::operator=(const ImpulsiveBurn &orig)
{
   if (this != &orig)
      Burn::operator=(orig);

   return *this;
}


//------------------------------------------------------------------------------
//  bool Fire(Real *burnData)
//------------------------------------------------------------------------------
/**
 * Applies the burn.  
 * 
 * Provides the mathematics that model an impulsive burn.  The parameter 
 * (burnData) can be used to pass in a Cartesian state (x, y, z, Vx, Vy, Vz) 
 * that needs to incorporate the burn.  If the class has an assigned Spacecraft,
 * that spacecraft is used instead of the input state.
 * 
 * @param <burnData>    Array of data specific to the derived burn class.
 *
 * @return true on success, throws on failure.
 */
//------------------------------------------------------------------------------
bool ImpulsiveBurn::Fire(Real *burnData, Real epoch)
{
   #ifdef DEBUG_IMPULSIVE_BURN
      MessageInterface::ShowMessage("ImpulsiveBurn::Fire entered for %s\n",
         instanceName.c_str());
   #endif

   frame = frameman->GetFrameInstance(coordAxes);
   if (frame == NULL)
      throw BurnException("Maneuver frame undefined");
    
   Real *satState;

   /// @todo Consolidate Finite & Impulsive burn initialization into base class
   if (sc)
      satState = sc->GetState().GetState();
   else
      throw BurnException("Maneuver initial state undefined (No spacecraft?)");
    
   Real state[6];

   if (epoch == 21545.0)
      epoch = sc->GetRealParameter("A1Epoch");
   
   TransformJ2kToBurnOrigin(satState, state, epoch);

   // Set the state 6-vector from the associated spacecraft
   frame->SetState(state);
   // Calculate the maneuver basis vectors
   frame->CalculateBasis(frameBasis);
    
   #ifdef DEBUG_IMPULSIVE_BURN
      MessageInterface::ShowMessage(
         "   Maneuvering spacecraft %s\n",
         sc->GetName().c_str());
      MessageInterface::ShowMessage(
         "   Position for burn:    %18le  %18le  %18le\n",
         (*state)[0], (*state)[1], (*state)[2]);
      MessageInterface::ShowMessage(
         "   Velocity before burn: %18le  %18le  %18le\n",
         (*state)[3], (*state)[4], (*state)[5]);
   #endif

   // Add in the delta-V
//   TransformDeltaVToJ2kFrame(deltaV, epoch);

   satState[3] += deltaV[0]*frameBasis[0][0] +
                  deltaV[1]*frameBasis[0][1] +
                  deltaV[2]*frameBasis[0][2];
   satState[4] += deltaV[0]*frameBasis[1][0] +
                  deltaV[1]*frameBasis[1][1] +
                  deltaV[2]*frameBasis[1][2];
   satState[5] += deltaV[0]*frameBasis[2][0] +
                  deltaV[1]*frameBasis[2][1] +
                  deltaV[2]*frameBasis[2][2];

   #ifdef DEBUG_IMPULSIVE_BURN
      MessageInterface::ShowMessage(
         "   Velocity after burn:  %18le  %18le  %18le\n",
         (*state)[3], (*state)[4], (*state)[5]);
   #endif

   return true;
}


//------------------------------------------------------------------------------
// void TransformDeltaVToJ2kFrame(Real *deltaV, Real epoch)
//------------------------------------------------------------------------------
void ImpulsiveBurn::TransformDeltaVToJ2kFrame(Real *deltaV, Real epoch)
{
   if ((j2000Body == NULL) || (burnOrigin == NULL))
      Initialize();
      
   if (j2000Body != burnOrigin)
   {
      Rvector6 j2kState = j2000Body->GetMJ2000State(epoch);
      Rvector6 originState = burnOrigin->GetMJ2000State(epoch);
      Rvector6 delta = j2kState - originState;
      
      deltaV[0] += delta[3];
      deltaV[1] += delta[4];
      deltaV[2] += delta[5];
   }      
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ImpulsiveBurn.
 *
 * @return clone of the ImpulsiveBurn.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ImpulsiveBurn::Clone() const
{
   return (new ImpulsiveBurn(*this));
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
void ImpulsiveBurn::Copy(const GmatBase* orig)
{
   operator=(*((ImpulsiveBurn *)(orig)));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference object name used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//---------------------------------------------------------------------------
bool ImpulsiveBurn::RenameRefObject(const Gmat::ObjectType type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   return Burn::RenameRefObject(type, oldName, newName);
}

