//$Id$
//------------------------------------------------------------------------------
//                               StateVector
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2005/06/02
//
/**
 * Implements the StateVector class
 */
//------------------------------------------------------------------------------

#include "StateVector.hpp"
#include "StateVectorException.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

// #define DEBUG_STATEVECTOR 1 

// StateVector constant variables for the lists of state types and elements 
const std::string StateVector::STATE_LIST[StateTypeCount] =
   {
      "Cartesian", "Keplerian", "ModifiedKeplerian", 
      "SphericalAZFPA", "SphericalRADEC"
   };

const std::string StateVector::ELEMENT_LIST[StateTypeCount][ElementTypeCount] =
   {
      {"X", "Y", "Z", "VX", "VY", "VZ","",""},
      {"SMA", "ECC", "INC", "RAAN", "AOP", "TA", "MA", "EA"},
      {"RadPer", "RadApo", "INC", "RAAN", "AOP", "TA", "MA", "EA"},
      {"RMAG", "RA", "DEC", "VMAG", "AZI", "FPA", "", ""},
      {"RMAG", "RA", "DEC", "VMAG", "RAV", "DECV", "", ""}
   };

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  StateVector()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 */
//---------------------------------------------------------------------------
StateVector::StateVector()
{
   DefineDefault();
}


//---------------------------------------------------------------------------
//  StateVector(const std::string &type)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <type> state type
 *
 */
//---------------------------------------------------------------------------
StateVector::StateVector(const std::string &type)
{
   DefineDefault();
    
   // Check if invalid then use default
   if (!SetValue(type))
      MessageInterface::ShowMessage("\n****Warning: Invalid state type ***"
                                    "\nUse default state values.\n");  
}


//---------------------------------------------------------------------------
//  StateVector(const Rvector6 stateVector)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <stateVector> state's values
 *
 */
//---------------------------------------------------------------------------
StateVector::StateVector(const Rvector6 stateVector)
{
   DefineDefault();
   mState = stateVector;
}


//---------------------------------------------------------------------------
//  StateVector(const std::string &type, const Rvector6 stateVector)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <type> state's type 
 * @param <stateVector> state's values
 *
 */
//---------------------------------------------------------------------------
StateVector::StateVector(const std::string &type, const Rvector6 stateVector)
{
   DefineDefault();

   // Check for invalid state type
   if (!SetValue(type, stateVector))
      MessageInterface::ShowMessage("\n****Warning: Invalid state type ***"
                                    "\nUse default state values.\n");  
}


//---------------------------------------------------------------------------
//  StateVector(const StateVector &sv)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base StateVector structures.
 *
 * @param <sv> The original that is being copied.
 */
//---------------------------------------------------------------------------
StateVector::StateVector(const StateVector &sv)
{
    InitializeDataMethod(sv);
}


//---------------------------------------------------------------------------
//  ~StateVector(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
StateVector::~StateVector(void)
{
}


//---------------------------------------------------------------------------
//  StateVector& operator=(const StateVector &sv)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateVector structures.
 *
 * @param <sv> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
StateVector& StateVector::operator=(const StateVector &sv)
{
   if (&sv == this)
      return *this;

   // Duplicate the member data        
   InitializeDataMethod(sv);

   return *this;
}


//---------------------------------------------------------------------------
//  Rvector6 GetValue() const
//---------------------------------------------------------------------------
/**
 * Retrieve the value.
 *
 * @return the state's value.
 */
//---------------------------------------------------------------------------
Rvector6 StateVector::GetValue() const
{
   return mState;
}


//---------------------------------------------------------------------------
//  Rvector6 GetValue(const std::string &type) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value with the specific state type.
 *
 * @return the state's value.
 */
//---------------------------------------------------------------------------
Rvector6 StateVector::GetValue(const std::string &type) const
{
   return (mStateConverter.Convert(mState, mStateType, type, (Anomaly&)mAnomaly));
}


//---------------------------------------------------------------------------
//  bool SetValue(const std::string &type)
//---------------------------------------------------------------------------
/**
 * Set the value with the specific state type.
 *
 * @param <type>  state type 
 *
 * @return true when successful; otherwise, false.
 */
//---------------------------------------------------------------------------
bool StateVector::SetValue(const std::string &type) 
{
   if (!IsValidType(type))
      return false; 
   
   if (mStateType != type)
   {
       try
       {
          mState = mStateConverter.Convert(mState, mStateType, type, mAnomaly);
          mStateType = type;
       }
       catch(UtilityException &)
       {
          return false; 
       }
   }
   
   return true;
}


//---------------------------------------------------------------------------
//  bool SetValue(const Rvector6 state)
//---------------------------------------------------------------------------
/**
 * Set the value with the specific state type.
 *
 * @param <state>  state value
 *
 * @return true when successful; otherwise, false.
 */
//---------------------------------------------------------------------------
bool StateVector::SetValue(const Rvector6 state) 
{
   mState = state;
   
   // @todo:  need to make sure with anomaly and others
   return true;
}


//---------------------------------------------------------------------------
//  bool SetValue(const std::string &type,const Rvector6 state)
//---------------------------------------------------------------------------
/**
 * Set the value with the specific state type.
 *
 * @param <type>  state type 
 * @param <state>  state value
 *
 * @return true when successful; otherwise, false.
 */
//---------------------------------------------------------------------------
bool StateVector::SetValue(const std::string &type, const Rvector6 state) 
{
   if (IsValidType(type))
      return false;

   mStateType = type;
   mState = state;
   
   // @todo:  need to make sure with anomaly and others
   return true;
}


//---------------------------------------------------------------------------
//  Real GetElement(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for element.
 *
 * @param <id> The element ID.
 *
 * @return the element's value.
 */
//---------------------------------------------------------------------------
Real StateVector::GetElement(const Integer id) const
{
   #if DEBUG_STATEVECTOR   
   MessageInterface::ShowMessage("\n*** StateVector::GetElement(%d) ****\n",id);
   #endif

   // Check for out of the range then throw exception
   if (id < 1 || id > 6)
      throw StateVectorException("StateVector::GetElement - out of range");
   
   return mState[id - 1];    
}


//---------------------------------------------------------------------------
//  Real GetElement(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for element.
 *
 * @param <label> The label of the element.
 *
 * @return the element's value.
 */
//---------------------------------------------------------------------------
Real StateVector::GetElement(const std::string &label) const
{
   // Find the state type
   std::string findType = FindType(label);

   #if DEBUG_STATEVECTOR   
   MessageInterface::ShowMessage("\n*** StateVector::GetElement(%s), findType "
                                 "= %s\n",label.c_str(), findType.c_str());
   #endif
   
   if (findType == "NoFound")
   {
      MessageInterface::ShowMessage("\nNo found due to invalid label.\n");
      throw StateVectorException("\nNo found due to invalid label.\n");
   }
   
   // First check with anomaly -> @todo: also need move into element1-6 below
   if (!mAnomaly.IsInvalid(label))
      return mAnomaly.GetValue(label);
   
   UnsignedInt id = GetElementID(label);
   
   if (mStateType == findType)
      return mState[id];
   
   // Do the conversion
   Rvector6 tempState = mStateConverter.Convert(mState, mStateType, findType,
                                                (Anomaly&)mAnomaly);
   
   return tempState[id];
}


//---------------------------------------------------------------------------
//  bool SetElement(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for element.
 *
 * @param <id> The integer ID for the element.
 * @param <value> The new element value.
 *
 * @return true if successful; otherwise, false
 */
//---------------------------------------------------------------------------
bool StateVector::SetElement(const Integer id, const Real value)
{
   #if DEBUG_STATEVECTOR
   MessageInterface::ShowMessage(
       "\n*** StateVector::SetElement(%d,%f) enters...\n\ttype = %s\n", 
       id, value, type.c_str());
   #endif

   // Check for the coordinate representation then set the value
   if (id < 1 || id > 6)
      return false;

   mState[id - 1] = value;
   return true;
}


//---------------------------------------------------------------------------
//  void SetElement(const std::string &label, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for element.
 *
 * @param <label> The label for the element.
 * @param <value> The new element value.
 *
 * @return true if successful; otherwise, false.
 */
//---------------------------------------------------------------------------
bool StateVector::SetElement(const std::string &label, const Real value)
{
   #if DEBUG_STATEVECTOR
   MessageInterface::ShowMessage(
       "\n*** StateVector::SetElement(%s,%f) with type (%s) enters...\n", 
       label.c_str(),value,type.c_str());
   #endif
   
   // Find the state type
   std::string findType = FindType(label);
   
   #if DEBUG_STATEVECTOR
   MessageInterface::ShowMessage("\nfindType = %s\n", findType.c_str());
   #endif
   
   if (findType == "NoFound")
   {
       MessageInterface::ShowMessage("\nStateVector::SetElement(%s,%f), "
                                     "label(%s) has no found.\n",
                                     label.c_str(), value, label.c_str());
       return false;
   }
   
   // Get the element id from the element's label
   UnsignedInt id = GetElementID(label);
   
   #if DEBUG_STATEVECTOR
   MessageInterface::ShowMessage("\nStateVector::SetElement..., id = %d\n",id);
   #endif
   
   // if different type from current then do conversion
   if (findType != mStateType)
   {
      try
      {
         mState = mStateConverter.Convert(mState, mStateType, findType, mAnomaly);
         mStateType = findType;
      } 
      catch(UtilityException &)
      {
         return false;
      }  
   }
   
   mState[id] = value;
   
   return true;
}


//---------------------------------------------------------------------------
//  std::string GetType() const
//---------------------------------------------------------------------------
/**
 * Get the state type.
 *
 * @return the element's type. 
 */
//---------------------------------------------------------------------------
std::string StateVector::GetType() const
{
   return mStateType;
}


//---------------------------------------------------------------------------
//  bool SetType(const std::string &type) 
//---------------------------------------------------------------------------
/**
 * Set the state type.
 *
 * @param <type> Given element's type. 
 *
 * @return true if successful; otherwise, false.
 */
//---------------------------------------------------------------------------
bool StateVector::SetType(const std::string &type)
{
   if (!SetValue(type))
      return false; 
  
   return true; 
}


//---------------------------------------------------------------------------
//  std::string GetLabel(const Integer id)
//---------------------------------------------------------------------------
/**
 * Determines the label of the element ID. 
 *
 * @param <id>  element ID
 *
 * @return the element's label 
 */
//---------------------------------------------------------------------------
std::string StateVector::GetLabel(const Integer id) const
{
   #if DEBUG_STATEVECTOR
   MessageInterface::ShowMessage("\n*** StateVector::GetLabel(%d)\n",id);
   #endif

   if (id < 1 || id > 6)
      throw StateVectorException("StateVector::GetElement - out of range");
 
   // Check for Cartesian
   if (mStateType == STATE_LIST[0])
   {
      switch (id)
      {
         case 1:   return "X";
         case 2:   return "Y";
         case 3:   return "Z";
         case 4:   return "VX";
         case 5:   return "VY";
         case 6:   return "VZ";
      }
   }
   
   // Check for Keplerian and Modified Keplerian
   if (mStateType == STATE_LIST[1] || mStateType == STATE_LIST[2])
   {
      switch (id)
      {
         case 1:   if (mStateType == STATE_LIST[1]) return "SMA";
                   return "RadPer";
         case 2:   if (mStateType == STATE_LIST[1]) return "ECC";
                   return "RadApo";
         case 3:   return "INC";
         case 4:   return "RAAN";
         case 5:   return "APO";
         case 6:   return mAnomaly.GetTypeString();
      }
   }

   // Check for Spherical with AZIFPA and RADEC
   if (mStateType == STATE_LIST[2] || mStateType == STATE_LIST[3])
   {
      switch (id)
      {
         case 1:   return "RMAG"; 
         case 2:   return "RA";
         case 3:   return "DEC";
         case 4:   return "VMAG";
         case 5:   if (mStateType == STATE_LIST[2]) return "AZI";
                   return "RAV";
         case 6:   if (mStateType == STATE_LIST[2]) return "FPA";
                   return "DECV";
      }
   }
   
   return "";   // Won't happen unless the state type or element is no found
}


//---------------------------------------------------------------------------
//  bool IsElement(const Integer id, const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Determines if the ID is for the element. 
 *
 * @param <id>     element's ID 
 * @param <label>  element's label 
 *
 * @return true if it is a part of element; otherwise,false.
 */
//---------------------------------------------------------------------------
bool StateVector::IsElement(const Integer id, const std::string &label) const
{
   if (id < 1 || id > 6)
      return false;
 
   for (UnsignedInt i=0; i < StateTypeCount; i++)
   {
      if (ELEMENT_LIST[i][id-1] == label)
         return true;

      else if (id == 6 && (i == KEPLERIAN || i == MODIFIED_KEPLERIAN)) 
      {
         for (UnsignedInt temp = id; temp < ElementTypeCount; ++temp)
         {
            if (ELEMENT_LIST[i][temp] == label)
               return true;
         }
      }
   }

   return false;
}


//---------------------------------------------------------------------------
//  bool IsElement(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Determines if the label is for the element. 
 *
 * @param <label>  element's label
 *
 * @return true if it is a part of element; otherwise,false.
 */
//---------------------------------------------------------------------------
bool StateVector::IsElement(const std::string &label) const
{
   #if DEBUG_STATEVECTOR
   MessageInterface::ShowMessage("\nStateVector::IsElement(%s)\n",
                                 label.c_str());
   #endif

   for (UnsignedInt i=0; i < StateTypeCount; i++)
   {
      for (UnsignedInt j=0; j < ElementTypeCount; j++)
      {
         if (ELEMENT_LIST[i][j] == label)
            return true;
      }
   }
   
   return false;
}


//---------------------------------------------------------------------------
//  bool SetAnomaly(const Rvector6 kepl,const std::string &type)
//---------------------------------------------------------------------------
/**
 * Set the anomaly. 
 *
 * @param <kepl>  Keplerian state
 * @param <type> anomaly type
 *
 * @return true if successful; otherwise,false.
 */
//---------------------------------------------------------------------------
bool StateVector::SetAnomaly(const Rvector6 kepl,const std::string &type)
{
   mAnomaly.Set(kepl[0], kepl[1], kepl[5], type);
  
   return true;
}


//---------------------------------------------------------------------------
//  std::string GetAnomalyType() const 
//---------------------------------------------------------------------------
/**
 * Get the anomaly type. 
 *
 * @return anomaly type.
 */
//---------------------------------------------------------------------------
std::string StateVector::GetAnomalyType() const
{
   return mAnomaly.GetTypeString();
}


//---------------------------------------------------------------------------
//  bool SetAnomalyType(const std::string &type) 
//---------------------------------------------------------------------------
/**
 * Set the anomaly type.
 *
 * @param <type> the anomaly type.
 *
 * @return true if successful; otherwise, false.
 */
//---------------------------------------------------------------------------
bool StateVector::SetAnomalyType(const std::string &type)
{
   try
   {
      mAnomaly.SetType(type);
   }
   catch(UtilityException &)
   {
      return false;
   }
   return true;
}


//---------------------------------------------------------------------------
//  bool IsValidType(const std::string &type) const
//---------------------------------------------------------------------------
/**
 * Check validity on the given state type. 
 *
 * @param <type> The state type.
 *
 * @return true if valid; otherwise, false.
 */
//---------------------------------------------------------------------------
bool StateVector::IsValidType(const std::string &type) const
{
   for (UnsignedInt i=0; i < 5; i++)
   {
       if (STATE_LIST[i] == type)
          return true;
   }
   return false;
}


//------------------------------------------------------------------------------
// bool SetCoordSys(const CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Set the coordinate system for setting the Mu.
 *
 * @return true if successful; otherwise false
 */
//---------------------------------------------------------------------------
bool StateVector::SetCoordSys(const CoordinateSystem *cs)
{
   return mStateConverter.SetMu(cs);
}

//-------------------------------------
// private methods
//-------------------------------------

//------------------------------------------------------------------------------
// void DefineDefault()
//------------------------------------------------------------------------------
/**
 * Initialize data method as default. 
 */
//---------------------------------------------------------------------------
void StateVector::DefineDefault()
{
   mStateType = "Cartesian";

   mState[0] = 7100.0;
   mState[1] = 0.0;
   mState[2] = 1300.0;
   mState[3] = 0.0;
   mState[4] = 7.35;
   mState[5] = 1.0;

   // Get the keplerian state and then initialize anomaly
   Rvector6 tempKepl = GetValue("Keplerian");  
   mAnomaly.Set(tempKepl[0], tempKepl[1], tempKepl[5], "TA");
}


//---------------------------------------------------------------------------
//  void InitializeDataMethod(const StateVector &s)
//---------------------------------------------------------------------------
/**
 * Initialize data method from the parameter for copying spacecraft structures.
 *
 * @param <s> The original that is being copied.
 */
//---------------------------------------------------------------------------
void StateVector::InitializeDataMethod(const StateVector &s)
{
    // Duplicate the member data
    mStateType = s.mStateType;
    mAnomaly = s.mAnomaly;
    mStateConverter = s.mStateConverter;
    mState = s.mState;
}


//---------------------------------------------------------------------------
// std::string FindType(const std::string &label)
//---------------------------------------------------------------------------
/** 
 * Determine the state type from the given element.
 *     
 * @param <label> Given specific element.
 *     
 * @return Getting the state type from the given element.
 */    
//---------------------------------------------------------------------------
std::string StateVector::FindType(const std::string &label) const
{            
   #if DEBUG_STATEVECTOR
   MessageInterface::ShowMessage("\n*** StateVector::FindType-> label: %s",
                                 label.c_str());
   #endif

   if (label == "X" || label == "Y" || label == "Z" ||
       label == "VX" || label == "VY" || label == "VZ")
   {
      return STATE_LIST[0];
   }

   if (label == "SMA" || label == "ECC" || label == "INC" || label == "RAAN" ||
       label == "AOP" || !(mAnomaly.IsInvalid(label)))
   {
      return STATE_LIST[1];
   }

   if (label == "RadPer" || label == "RadApo")
      return STATE_LIST[2];

   if (label == "RMAG" || label == "RA" || label == "DEC" || label == "VMAG" ||
       label == "AZI" || label == "FPA")
   {
      return STATE_LIST[3];
   }

   if (label == "RAV" || label == "DECV")
      return STATE_LIST[4];

   return "NoFound";
}


//---------------------------------------------------------------------------
// UnsignedInt GetElementID(const std::string &label) const
//---------------------------------------------------------------------------
/** 
 * Determine the state type from the given element.
 *     
 * @param <label> Given specific element.
 *     
 * @return the element ID.
 */    
//---------------------------------------------------------------------------
UnsignedInt StateVector::GetElementID(const std::string &label) const
{
   if (label == "X" || label == "SMA" || label == "RadPer" || label == "RMAG")
      return 0;

   if (label == "Y" || label == "ECC" || label == "RadApo" || label == "RA")
      return 1;

   if (label == "Z" || label == "INC" || label == "DEC")
      return 2;

   if (label == "VX" || label == "RAAN" || label == "VMAG")
      return 3;

   if (label == "VY" || label == "AOP" || label == "AZI" || label == "RAV")
      return 4;

   if (label == "VZ" || label == "FPA" || label == "DECV" || 
       !mAnomaly.IsInvalid(label))
      return 5;
   
   // Use default if no found
   return 0;
}
