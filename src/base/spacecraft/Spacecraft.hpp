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

#include "SpaceObject.hpp"
#include "SolarSystem.hpp"
#include "Rvector6.hpp"
#include "StateConverter.hpp"
#include "TimeConverter.hpp"
#include "MessageInterface.hpp"
#include "PropState.hpp"

class GMAT_API Spacecraft : public SpaceObject
{
   enum SC_Param_ID 
   {
      //EPOCH_ID = SpaceObjectParamCount, ELEMENT1_ID, ELEMENT2_ID, ELEMENT3_ID, ELEMENT4_ID,
      ELEMENT1_ID = SpaceObjectParamCount, ELEMENT2_ID, ELEMENT3_ID, ELEMENT4_ID,
      ELEMENT5_ID, ELEMENT6_ID, STATE_TYPE_ID, BODY_ID, FRAME_ID, PLANE_ID, 
      DRY_MASS_ID,DATE_FORMAT_ID, COEFF_DRAG_ID, DRAG_AREA_ID, SRP_AREA_ID,
      REFLECT_COEFF_ID, /*NO_NAME,*/ SC_ParameterIDs
   };
   
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

   // Parameter accessor methods -- overridden from GmatBase
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
    
   virtual std::string GetParameterText(const Integer id) const;
   virtual Gmat::ParameterType
   GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   //    virtual PropState& GetState(void); 

   void SetState(const std::string &elementType, Real *instate);
   void SetState(const Real s1, const Real s2, const Real s3, 
                 const Real s4, const Real s5, const Real s6);

   Rvector6 GetCartesianState();
   Rvector6 GetKeplerianState();
   Rvector6 GetModifiedKeplerianState();
   // Will add more methods below later
   // Rvector6 GetSphericalOneState() const;
   // Rvector6 GetSphericalTwoState() const;

   bool GetDisplay() const;
   void SetDisplay(const bool displayFlag);

   std::string GetDisplayDateFormat() const;
   void SetDisplayDateFormat(const std::string &dateType);
   std::string GetDisplayEpoch();
   bool SetDisplayEpoch(const std::string &value);

   std::string GetDisplayCoordType() const;
   void SetDisplayCoordType(const std::string &coordType);

   Real* GetDisplayState();
   void SetDisplayState(const Real *s);
   void SetDisplayState(const Rvector6 s);
   void SaveDisplay();

   virtual SolarSystem* GetSolarSystem() const;
   virtual void SetSolarSystem(SolarSystem *ss);
   
   // Default values for spacecraft 
   static const Real EPOCH; 
   static const Real ELEMENT1; 
   static const Real ELEMENT2; 
   static const Real ELEMENT3; 
   static const Real ELEMENT4; 
   static const Real ELEMENT5; 
   static const Real ELEMENT6; 
   static const std::string DATEFORMAT; 
   static const std::string STATE_TYPE; 
   static const std::string REF_BODY; 
   static const std::string REF_FRAME; 
   static const std::string REF_PLANE; 

protected:
   // Declare protetced method data of internal spacecraft information
   // Real           epoch;      // Moved to SpaceObject  
   // DJC:  7/21/04 Update for the state vector used in propagation
   // Real           state[6];
   // PropState      state;      // Moved to SpaceObject
   Real           dryMass;
   Real           coeffDrag;
   Real           dragArea;
   Real           srpArea;
   Real           reflectCoeff;
   std::string    subType;      // @todo will add it later
   std::string    dateFormat;
   std::string    stateType;   
   std::string    refBody; 
   std::string    refFrame;   
   std::string    refPlane; 
   //Integer        epochID;
   Integer        state1ID;
   Integer        state2ID;
   Integer        state3ID;
   Integer        state4ID;
   Integer        state5ID;
   Integer        state6ID;       
   Integer        subTypeID;     // @todo will add it later
   Integer        stateTypeID;
   Integer        refBodyID; 
   Integer        refFrameID; 
   Integer        refPlaneID; 
   Integer        dryMassID;
   Integer        dateFormatID;
   Integer        coeffDragID;
   Integer        dragAreaID;
   Integer        srpAreaID;
   Integer        reflectCoeffID;

   // for non-internal spacecraft information
   SolarSystem    *solarSystem;
   StateConverter stateConverter;
   TimeConverter  timeConverter;
   Rvector6       cartesianState;
   Rvector6       keplerianState;
   Rvector6       modifiedKeplerianState;
   Rvector6       sphericalOneState;
   Rvector6       sphericalTwoState;
   bool           isForDisplay;
   std::string    displayEpoch;
   Real           displayState[6];
   std::string    displayDateFormat;
   std::string    displayCoordType; 
   std::string    displaySubType;
   
private:
   bool        initialDisplay;
   bool        hasElements[6];
   void        InitializeValues();
   void        SetEpoch();
   std::string GetElementName(const Integer id) const; 
   std::string GetLocalCoordType() const;
   void        SetInitialDisplay();
   std::string ToString(const Real value);
   
   void        SetInitialState(); //loj: 10/25/04 added
};

#endif // Spacecraft_hpp
