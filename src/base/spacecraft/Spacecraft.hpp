//$Id$
//------------------------------------------------------------------------------
//                                 Spacecraft
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus, Reworked by D. Conway
// Created: 2003/10/22
//
/**
 * Definition of the Spacecraft class base
 */
//------------------------------------------------------------------------------

#ifndef Spacecraft_hpp
#define Spacecraft_hpp

#include <valarray>
#include "SpaceObject.hpp"
#include "Rvector6.hpp"
#include "PropState.hpp"
#include "FuelTank.hpp"
#include "Thruster.hpp"
#include "Anomaly.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "TimeSystemConverter.hpp"
#include "StateConverter.hpp"
#include "Attitude.hpp"

#include <map>

class GMAT_API Spacecraft : public SpaceObject
{
public:
   Spacecraft(const std::string &name, 
      const std::string &typeStr = "Spacecraft");
   Spacecraft(const Spacecraft &a);
   Spacecraft&          operator=(const Spacecraft &a);
   
   // Destructor
   virtual              ~Spacecraft();

   CoordinateSystem*    GetInternalCoordSystem();
   void                 SetInternalCoordSystem(CoordinateSystem *cs);
   
   void                 SetState(const Rvector6 &cartState);
   void                 SetState(const std::string &elementType, Real *instate);
   void                 SetState(const Real s1, const Real s2, const Real s3, 
                                 const Real s4, const Real s5, const Real s6);
   
   virtual PropState&   GetState();
   virtual Rvector6     GetState(std::string rep);
   virtual Rvector6     GetState(Integer rep);
   Rvector6             GetCartesianState();
   Rvector6             GetKeplerianState();
   Rvector6             GetModifiedKeplerianState();
   
   Anomaly              GetAnomaly() const;
   
   Rmatrix33            GetAttitude(Real a1mjdTime) const;
   Rvector3             GetAngularVelocity(Real a1mjdTime) const;
   UnsignedIntArray     GetEulerAngleSequence() const;
   
   // inherited from GmatBase
   virtual GmatBase*    Clone(void) const;
   virtual void         Copy(const GmatBase* orig);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   
   virtual const        ObjectTypeArray& GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObjectName(const Gmat::ObjectType type,
                                         const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);
   
   // Parameter accessor methods -- overridden from GmatBase
   virtual Integer      GetParameterID(const std::string &str) const;
   
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const Integer id, const Real value);
   virtual Real         SetRealParameter(const std::string &label, const Real value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id, const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   
   const StringArray&   GetStringArrayParameter(const Integer id) const;
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual bool         Initialize();
   
   virtual bool         TakeAction(const std::string &action, 
                                   const std::string &actionData = "");
   virtual bool         IsOwnedObject(Integer id) const;
   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   
   
   virtual const std::string&  
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   std::string GetEpochString();
   void SetDateFormat(const std::string &dateType);   
   void SetEpoch(const std::string &ep);
   void SetEpoch(const std::string &type, const std::string &ep, Real a1mjd);
   void SetState(const std::string &type, const Rvector6 &cartState);
   void SetAnomaly(const std::string &type, const Anomaly &ta);
   
protected:
   enum SC_Param_ID 
   {
      SC_EPOCH_ID = SpaceObjectParamCount,
      ELEMENT1_ID, 
      ELEMENT2_ID, 
      ELEMENT3_ID, 
      ELEMENT4_ID, 
      ELEMENT5_ID, 
      ELEMENT6_ID, 
      ELEMENT1UNIT_ID, 
      ELEMENT2UNIT_ID, 
      ELEMENT3UNIT_ID, 
      ELEMENT4UNIT_ID, 
      ELEMENT5UNIT_ID, 
      ELEMENT6UNIT_ID, 
      STATE_TYPE_ID,           // deprecated
      DISPLAY_STATE_TYPE_ID, 
      ANOMALY_ID, 
      COORD_SYS_ID,
      DRY_MASS_ID,
      DATE_FORMAT_ID, 
      CD_ID, 
      CR_ID, 
      DRAG_AREA_ID, 
      SRP_AREA_ID,
      FUEL_TANK_ID, 
      THRUSTER_ID, 
      TOTAL_MASS_ID, 
      ATTITUDE,
      SpacecraftParamCount
   };
   
   enum MultipleReps  // these are IDs for the different representations
   {
      CART_X = 10000,      // Cartesian
      CART_Y,
      CART_Z,
      CART_VX,
      CART_VY,
      CART_VZ,
      KEPL_SMA,            // Keplerian
      KEPL_ECC,
      KEPL_INC,
      KEPL_RAAN,
      KEPL_AOP,
      KEPL_TA,
      KEPL_EA,
      KEPL_MA,
      KEPL_HA,
      MOD_KEPL_RADPER,     // Modified Keplerian
      MOD_KEPL_RADAPO,
      AZFPA_RMAG,          // SphericalAZFPA
      AZFPA_RA,
      AZFPA_DEC,
      AZFPA_VMAG,
      AZFPA_AZI,
      AZFPA_FPA,
      RADEC_RAV,           // SphericalRADEC
      RADEC_DECV,
      EQ_PEY,              // Equinoctial
      EQ_PEX,
      EQ_PNY,
      EQ_PNX,
      EQ_MLONG, 
      EndMultipleReps    
   };
   // these are the corresponding strings
   static const std::string MULT_REP_STRINGS[EndMultipleReps - CART_X];

   /// Spacecraft parameter types
   static const Gmat::ParameterType 
                  PARAMETER_TYPE[SpacecraftParamCount - SpaceObjectParamCount];
   /// Spacecraft parameter labels
   static const std::string 
                  PARAMETER_LABEL[SpacecraftParamCount - SpaceObjectParamCount];
   
   enum STATE_REPS
   {
      CARTESIAN_ID = 0,
      KEPLERIAN_ID,
      MODIFIED_KEPLERIAN_ID,
      SPHERICAL_AZFPA_ID,
      SPHERICAL_RADEC_ID
   };
   
   static const Integer ATTITUDE_ID_OFFSET;

   std::map <std::string, std::string> elementLabelMap;
   
   /// State element labels
   StringArray       stateElementLabel;
   /// State element units
   StringArray       stateElementUnits;
   /// Possible state representations
   StringArray       representations;
   
   /// Epoch string, specifying the text form of the epoch
   std::string       scEpochStr;
   Real              dryMass;
   Real              coeffDrag;
   Real              dragArea;
   Real              srpArea;
   Real              reflectCoeff;
   /// String specifying the epoch time system (A1, TAI, UTC, or TT)
   std::string       epochSystem;
   /// String specifying the epoch time format (Gregorian or ModJulian)
   std::string       epochFormat;
   /// String specifying the epoch system and format used for scEpochStr (TAIModJulian, etc)
   std::string       epochType;
   std::string       stateType;
   std::string       displayStateType;
   std::string       anomalyType;
   Anomaly           trueAnomaly;
   
   /// Base coordinate system for the Spacecraft
   CoordinateSystem  *internalCoordSystem;
   /// Coordinate system used for the input and output to the GUI
   CoordinateSystem  *coordinateSystem;
   
   std::string       coordSysName;
   
   /// Pointer to the object that manages the attitude of the spacecraft
   Attitude          *attitude;
   
   // for non-internal spacecraft information
   StateConverter    stateConverter;
   CoordinateConverter coordConverter;
   
   // Lists of hardware elements added 11/12/04, djc
   /// Fuel tank names
   StringArray       tankNames;
   /// Thruster names
   StringArray       thrusterNames;
   /// Pointers to the fuel tanks
   ObjectArray       tanks;
   /// Pointers to the spacecraft thrusters
   ObjectArray       thrusters;
   /// Dry mass plus fuel masses, a calculated parameter
   Real              totalMass;
   
   // New constructs needed to preserve interfaces
   Rvector6          rvState;

   // protected methods
   Real              UpdateTotalMass();
   Real              UpdateTotalMass() const;
   
   bool              initialDisplay;
   bool              csSet;

   virtual void      WriteParameters(Gmat::WriteMode mode, std::string &prefix, 
                        std::stringstream &stream);
                                
   virtual void      UpdateElementLabels();
   Rvector6          GetStateInRepresentation(std::string rep = "");
   Rvector6          GetStateInRepresentation(Integer rep = CARTESIAN_ID);
   void              SetStateFromRepresentation(std::string rep, Rvector6 &st);
   
   Real              GetElement(const std::string &label);
   bool              SetElement(const std::string &label, const Real &value);
   Integer           LookUpLabel(const std::string &label, std::string &rep);
   Integer           LookUpID(const Integer id, std::string &label, std::string &rep);
   void              BuildElementLabelMap();
};

#endif // Spacecraft_hpp
