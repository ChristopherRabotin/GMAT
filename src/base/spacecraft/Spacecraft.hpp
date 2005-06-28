//$Header$
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
// Author:  Joey Gurganus
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
#include "CoordinateSystem.hpp"
#include "Rvector6.hpp"
#include "Epoch.hpp"
#include "StateVector.hpp"
#include "TimeConverter.hpp"
#include "PropState.hpp"
#include "FuelTank.hpp"
#include "Thruster.hpp"

class GMAT_API Spacecraft : public SpaceObject
{
public:
   // Default constructor
   Spacecraft();
   Spacecraft(const std::string &name);
   Spacecraft(const std::string &typeStr, const std::string &name);
   // Copy constructor
   Spacecraft(const Spacecraft &a);
   // Assignment operator
   Spacecraft& operator=(const Spacecraft &a);

   // Destructor
   virtual ~Spacecraft(void);

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   virtual void Copy(const GmatBase* orig);
   
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;

   //  Tanks and thrusters (and, eventually, other hardware) are owned objects,
   // and therefore need these methods from GmatBase
   virtual const StringArray&
                       GetRefObjectNameArray(const Gmat::ObjectType type);
  virtual bool        SetRefObjectName(const Gmat::ObjectType type,
                                       const std::string &name);
   virtual GmatBase*   GetRefObject(const Gmat::ObjectType type,
                                    const std::string &name);
   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name = "");
//   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                    const std::string &name,
//                                    const Integer index);
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   // Parameter accessor methods -- overridden from GmatBase
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);

   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label, 
                                   const std::string &value);

   const StringArray& GetStringArrayParameter(const Integer id) const;
    
   virtual std::string GetParameterText(const Integer id) const;
   virtual Gmat::ParameterType
   GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual bool Initialize();

   virtual bool TakeAction(const std::string &action, 
                           const std::string &actionData = "");
   

   //    virtual PropState& GetState(void); 
   // @todo will work on GetStateVector()
   Rvector6 GetStateVector() const;
   Rvector6 GetStateVector(const std::string &elementType) const;
   bool     SetStateVector(const Rvector6 instate);
   bool     SetStateVector(const std::string &elementType,
                           const Rvector6 instate);

protected:
   enum SC_Param_ID 
   {
      // EPOCH_ID = SpaceObjectParamCount, 
      ELEMENT1_ID = SpaceObjectParamCount, ELEMENT2_ID, ELEMENT3_ID, 
      ELEMENT4_ID, ELEMENT5_ID, ELEMENT6_ID, 
      STATE_TYPE_ID, ANOMALY_ID, COORD_SYS_ID,
      DRY_MASS_ID, CD_ID, CR_ID, DRAG_AREA_ID, SRP_AREA_ID,
      FUEL_TANK_ID, THRUSTER_ID, TOTAL_MASS_ID, 
      SC_ParamCount
   };

   // Spacecraft parameter types
   static const Gmat::ParameterType 
          PARAMETER_TYPE[SC_ParamCount - SpaceObjectParamCount];
   
   // Declare protetced method data of internal spacecraft information
   // Real           epoch;      // Moved to SpaceObject  
   // DJC:  7/21/04 Update for the state vector used in propagation
   // Real           state[6];
   // PropState      state;      // Moved to SpaceObject
//   Epoch             epoch;
   StateVector       stateVector;
   Real              dryMass;
   Real              coeffDrag;
   Real              dragArea;
   Real              srpArea;
   Real              reflectCoeff;
   CoordinateSystem  *coordinateSystem;
   std::string       coordSysName;

   // Lists of hardware elements added 11/12/04, djc
   /// Fuel tank names
   StringArray    tankNames;
   /// Thruster names
   StringArray    thrusterNames;
   /// Pointers to the fuel tanks
   ObjectArray    tanks;
   /// Pointers to the spacecraft thrusters
   ObjectArray    thrusters;
          
   /// Dry mass plus fuel masses, a calculated parameter
   Real           totalMass;
   
   // protected methods
   Real           UpdateTotalMass();
   Real           UpdateTotalMass() const;
   
private:
   // private methods
   void        SetPropState();

   void        DefineDefaultSpacecraft(); //loj: 4/28/05 Added
   void        InitializeDataMethod(const Spacecraft &s);
};

#endif // Spacecraft_hpp
