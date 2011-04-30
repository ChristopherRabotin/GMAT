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
 * Definition of the StateVector class
 */
//------------------------------------------------------------------------------

#ifndef StateVector_hpp
#define StateVector_hpp

#include "gmatdefs.hpp"
#include "Rvector6.hpp"
#include "StateConverter.hpp"
#include "Anomaly.hpp"
#include "CoordinateSystem.hpp"

class GMAT_API StateVector
{
public:
   // Default constructor
   StateVector();
   StateVector(const std::string &type);
   StateVector(const Rvector6 stateVector);
   StateVector(const std::string &type, const Rvector6 stateVector);

   // Copy constructor
   StateVector(const StateVector &sv);
   // Assignment operator
   StateVector& operator=(const StateVector &sv);

   // Destructor
   virtual ~StateVector(void);

   // public methods
   Rvector6    GetValue() const;
   Rvector6    GetValue(const std::string &type) const;
   bool        SetValue(const std::string &type);
   bool        SetValue(Rvector6 state);
   bool        SetValue(const std::string &type, Rvector6 state);

   Real        GetElement(const Integer id) const;
   Real        GetElement(const std::string &label) const;
   bool        SetElement(const Integer id, const Real value);
   bool        SetElement(const std::string &label, const Real value); 

   std::string GetType() const;
   bool        SetType(const std::string &type);

   bool        IsValidType(const std::string &label) const;

   bool        IsElement(const Integer id, const std::string &label) const;
   bool        IsElement(const std::string &label) const;
   std::string GetLabel(const Integer id) const;

   bool        SetAnomaly(const Rvector6 kepl,const std::string &type);
   std::string GetAnomalyType() const;
   bool        SetAnomalyType(const std::string &type);

   bool        SetCoordSys(const CoordinateSystem *cs);

protected:
   // StateVector's element list
   enum StateType 
   {
        CARTESIAN, KEPLERIAN, MODIFIED_KEPLERIAN,
        SPHERICAL_AZFPA, SPHERICAL_RADEC, StateTypeCount
   };
   enum ElementType 
   {
        ELEMENT1, ELEMENT2, ELEMENT3, ELEMENT4, ELEMENT5, ELEMENT6,
        EXTRA_ELEMENT1, EXTRA_ELEMENT2, ElementTypeCount
   };
          
   static const std::string ELEMENT_LIST[StateTypeCount][ElementTypeCount];
   static const std::string STATE_LIST[StateTypeCount];
   
private:
   std::string             mStateType;
   Rvector6                mState;
   Anomaly                 mAnomaly;
   mutable StateConverter  mStateConverter;

   // private methods
   void        DefineDefault();
   void        InitializeDataMethod(const StateVector &s);
   std::string FindType(const std::string &label) const;
   UnsignedInt GetElementID(const std::string &label) const;

};

#endif // StateVector_hpp
