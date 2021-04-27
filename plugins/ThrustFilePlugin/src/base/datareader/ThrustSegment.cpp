//$Id$
//------------------------------------------------------------------------------
//                           ThrustSegment
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 5, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "ThrustSegment.hpp"
#include "MessageInterface.hpp"
#include "ODEModelException.hpp"
#include "StringUtil.hpp"

//#define DEBUG_TSF_SOLVEFOR
//#define DEBUG_SOLVE_FOR_PARAMETERS

//------------------------------------------------------------------------------
// Field names and types
//------------------------------------------------------------------------------

/// Script labels for the class fields
const std::string ThrustSegment::PARAMETER_LABEL[ThrustSegmentParamCount - GmatBaseParamCount] =
{
   "ThrustScaleFactor",          // THRUSTSCALEFACTOR
   "ThrustScaleFactorSigma",     // TSF_SIGMA
   "ApplyThrustScaleToMassFlow", // TSF_MASSFLOW
   "MassFlowScaleFactor",        // MASSFLOWSCALEFACTOR
   "MassSource",                 // MASSSOURCE
   "SolveFors",                  // SOLVEFORS
   "TSF_Epsilon",                // TSF_EPSILON
   "StartEpoch",                 // START_EPOCH
   "EndEpoch"                    // END_EPOCH
};

/// Types for each field
const Gmat::ParameterType ThrustSegment::PARAMETER_TYPE[ThrustSegmentParamCount - GmatBaseParamCount] =
{
   Gmat::REAL_TYPE,              // THRUSTSCALEFACTOR
   Gmat::REAL_TYPE,              // TSF_SIGMA
   Gmat::BOOLEAN_TYPE,           // TSF_MASSFLOW
   Gmat::REAL_TYPE,              // MASSFLOWSCALEFACTOR
   Gmat::STRINGARRAY_TYPE,       // MASSSOURCE
   Gmat::STRINGARRAY_TYPE,       // SOLVEFORS
   Gmat::REAL_TYPE,              // TSF_EPSILON
   Gmat::REAL_TYPE,              // START_EPOCH
   Gmat::REAL_TYPE               // END_EPOCH
};



//------------------------------------------------------------------------------
// ThrustSegment(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name used for this ThrustSegment
 */
//------------------------------------------------------------------------------
ThrustSegment::ThrustSegment(const std::string &name) :
   GmatBase                (Gmat::INTERFACE, "ThrustSegment", name),
   thrustScaleFactor       (1.0),
   tsfEpsilon              (0.0),
   tsfSigma                (1.0e70),
   depleteMass             (false),
   useMassAndThrustFactor  (false),
   massFlowFactor          (1.0),
   tsfIndex                (-1)
{
   objectTypes.push_back(Gmat::INTERFACE);
   objectTypeNames.push_back("Interface");
   objectTypeNames.push_back("ThrustSegment");
   parameterCount = ThrustSegmentParamCount;

   // Set default value to covariance matrix
   covariance.AddCovarianceElement("TSF_Epsilon", this);
   Real TSF_EpsilonSigma = tsfSigma / thrustScaleFactor;
   Rvector value(1, TSF_EpsilonSigma*TSF_EpsilonSigma);
   covariance.ConstructRHS(value, 0);
}

//------------------------------------------------------------------------------
// ~ThrustSegment()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThrustSegment::~ThrustSegment()
{
}

//------------------------------------------------------------------------------
// ThrustSegment(const ThrustSegment& ts) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ts The thrust segment copied to this one
 */
//------------------------------------------------------------------------------
ThrustSegment::ThrustSegment(const ThrustSegment& ts) :
   GmatBase                (ts),
   thrustScaleFactor       (ts.thrustScaleFactor),
   tsfEpsilon              (ts.tsfEpsilon),
   tsfSigma                (ts.tsfSigma),
   depleteMass             (ts.depleteMass),
   useMassAndThrustFactor  (ts.useMassAndThrustFactor),
   massFlowFactor          (ts.massFlowFactor),
   tsfIndex                (ts.tsfIndex)
{
   segData = ts.segData;
   massSourceNames = ts.massSourceNames;
   massSource = ts.massSource;
   solveFors = ts.solveFors;

   // This step was done in GmatBase(ts)                      // made changes by TUAN NGUYEN
   ////// Set TSF covariance
   ////Integer locationStart = covariance.GetSubMatrixLocationStart("TSF_Epsilon");          // This step may causes crash due to TSF_Epsilon was not added to covariance
   ////Real TSF_EpsilonSigma, tsf0;
   ////tsf0 = thrustScaleFactor / (1 + tsfEpsilon);
   ////TSF_EpsilonSigma = tsfSigma / tsf0;
   ////covariance(locationStart, locationStart) = TSF_EpsilonSigma * TSF_EpsilonSigma;
}

//------------------------------------------------------------------------------
// ThrustSegment& operator=(const ThrustSegment& ts)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ts The thrust segment copied to this one
 *
 * @return This segment, set to match ts.
 */
//------------------------------------------------------------------------------
ThrustSegment& ThrustSegment::operator=(const ThrustSegment& ts)
{
   if (this != &ts)
   {
      GmatBase::operator=(ts);

      thrustScaleFactor      = ts.thrustScaleFactor;
      tsfEpsilon             = ts.tsfEpsilon;
      tsfSigma               = ts.tsfSigma;
      depleteMass            = ts.depleteMass;
      useMassAndThrustFactor = ts.useMassAndThrustFactor;
      massFlowFactor         = ts.massFlowFactor;
      tsfIndex               = ts.tsfIndex;
      segData                = ts.segData;
      massSourceNames        = ts.massSourceNames;
      massSource             = ts.massSource;
      solveFors              = ts.solveFors;
   }

   // This step was done in GmatBase::operator=(ts);                               // made changes by TUAN NGUYEN
   ////// Set TSF covariance
   ////Integer locationStart = covariance.GetSubMatrixLocationStart("TSF_Epsilon");
   ////Real TSF_EpsilonSigma, tsf0;
   ////tsf0 = thrustScaleFactor / (1 + tsfEpsilon);
   ////TSF_EpsilonSigma = tsfSigma / tsf0;
   ////covariance(locationStart, locationStart) = TSF_EpsilonSigma * TSF_EpsilonSigma;

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of the segment
 *
 * @return A new ThrustSegment, matching this one
 */
//------------------------------------------------------------------------------
GmatBase* ThrustSegment::Clone() const
{
   return new ThrustSegment(*this);
}


//------------------------------------------------------------------------------
// bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether GetRefObjectTypeArray() is implemented.
 *
 * @return true if an array will be returned
 */
 //------------------------------------------------------------------------------
bool ThrustSegment::HasRefObjectTypeArray()
{
   return true;
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
const ObjectTypeArray& ThrustSegment::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::FUEL_TANK);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * This method returns an array with the names of the referenced objects.
 *
 * @param type Type of the object.
 *
 * @return a vector with the names of objects of the requested type.
 */
 //------------------------------------------------------------------------------
const StringArray& ThrustSegment::GetRefObjectNameArray(const UnsignedInt type)
{

   refObjectNames.clear();
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::FUEL_TANK)
   {
      // Add Tank names
      refObjectNames.insert(refObjectNames.end(), massSourceNames.begin(), massSourceNames.end());
   }

   return refObjectNames;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames a referenced object.
 *
 * @param type Type of the object.
 * @param oldName Old name of the object.
 * @param newName New name of the object.
 *
 * @return true
 */
 //------------------------------------------------------------------------------
bool ThrustSegment::RenameRefObject(const UnsignedInt type,
   const std::string &oldName,
   const std::string &newName)
{
   if (type == Gmat::FUEL_TANK)
   {
      for (UnsignedInt i = 0; i < massSourceNames.size(); i++)
      {
         if (massSourceNames[i] == oldName)
         {
            massSourceNames[i] = newName;
            break;
         }
      }
   }

   return true;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets referenced objects.
 *
 * @param obj The object.
 * @param type Type of the object.
 * @param name Name of the object.
 *
 * @return true if the ref object was set, false if not.
 */
 //------------------------------------------------------------------------------
bool ThrustSegment::SetRefObject(GmatBase *obj, const UnsignedInt type,
   const std::string &name)
{
   if (obj == NULL)
      return false;

   if (obj->IsOfType("FuelTank"))
   {
      if (find(massSource.begin(), massSource.end(), obj) == massSource.end())
      {
         // Replace old tank with new one. We don't want to delete the
         // old tank here since Spacecraft owns it (tank is not cloned in the
         // copy constructor)
         bool replaced = false;
         for (UnsignedInt i = 0; i < massSource.size(); i++)
         {
            if (massSource[i]->GetName() == name)
            {
               massSource[i] = (FuelTank*)obj;
               replaced = true;
            }
         }
         if (!replaced)
            massSource.push_back((FuelTank*)obj);
      }

      return true;
   }

   return GmatBase::SetRefObject(obj, type, name);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const UnsignedInt type)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 *
 * @param type The type of objects requested
 *
 * @return Reference to the array.
 */
 //---------------------------------------------------------------------------
ObjectArray& ThrustSegment::GetRefObjectArray(const UnsignedInt type)
{
   if (type == Gmat::FUEL_TANK)
   {
      objectArray.clear();
      for (UnsignedInt i = 0; i < massSource.size(); ++i)
         objectArray.push_back(massSource[i]);
      return objectArray;
   }

   return GmatBase::GetRefObjectArray(type);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const std::string& typeString)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers based on a string (e.g. the typename).
 *
 * @param typeString The string used to find the objects requested.
 *
 * @return Reference to the array.  This default method returns an empty vector.
 */
 //---------------------------------------------------------------------------
ObjectArray& ThrustSegment::GetRefObjectArray(const std::string& typeString)
{
   if ((typeString == "FuelTank") || (typeString == "Tanks"))
      return GetRefObjectArray(Gmat::FUEL_TANK);

   return GmatBase::GetRefObjectArray(type);
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the description for the parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return String description for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ThrustSegment::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ThrustSegmentParamCount)
      return PARAMETER_LABEL[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieve the ID for the parameter given its description.
 *
 * @param str Description for the parameter.
 *
 * @return the parameter ID, or -1 if there is no associated ID.
 */
//------------------------------------------------------------------------------
Integer ThrustSegment::GetParameterID(const std::string &str) const
{
   Integer id = -1;

   for (Integer i = GmatBaseParamCount; i < ThrustSegmentParamCount; i++)
    {
       if (str == PARAMETER_LABEL[i - GmatBaseParamCount])
       {
          id = i;
          break;
       }
    }

    if (id != -1)
       return id;

    return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter, or
 *         UNKNOWN_PARAMETER_TYPE.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ThrustSegment::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ThrustSegmentParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the string associated with a parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return Text description for the type of the parameter, or the empty
 *         string ("").
 */
//------------------------------------------------------------------------------
std::string ThrustSegment::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param id Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::IsParameterReadOnly(const Integer id) const
{
   if (id == TSF_EPSILON)
      return true;

   if (id == START_EPOCH || id == END_EPOCH)
      return true;

   return GmatBase::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param label Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::GetRealParameter(const Integer id) const
{
   if (id == THRUSTSCALEFACTOR)
       return thrustScaleFactor * (1.0 + tsfEpsilon);

   if (id == TSF_EPSILON)
       return tsfEpsilon;

   if (id == TSF_SIGMA)
      return tsfSigma;

   if (id == MASSFLOWSCALEFACTOR)
      return massFlowFactor;

   if (id == START_EPOCH)
      return hasPrecisionTime ? segData.startEpochGT.GetMjd() : segData.startEpoch;

   if (id == END_EPOCH)
      return hasPrecisionTime ? segData.endEpochGT.GetMjd() : segData.endEpoch;

   return GmatBase::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::SetRealParameter(const Integer id, const Real value)
{
   if (id == THRUSTSCALEFACTOR)
   {
      thrustScaleFactor = value;
      tsfEpsilon = 0.0;
      return thrustScaleFactor;
   }

   if (id == TSF_SIGMA)
   {
      if (value <= 0.0)
         throw ODEModelException("Error: a nonpositive number was set to ThrustScaleFactorSigma. A valid value has to be a positive number.\n");

      tsfSigma = value;

      Real TsfEpsilonSigma, tsf0;
      tsf0 = thrustScaleFactor / (1 + tsfEpsilon);
      TsfEpsilonSigma = tsfSigma / tsf0;

      Integer locationStart = covariance.GetSubMatrixLocationStart("TSF_Epsilon");
      covariance(locationStart, locationStart) = TsfEpsilonSigma * TsfEpsilonSigma;

      return tsfSigma;
   }

   if (id == TSF_EPSILON)
   {
       tsfEpsilon = value;
       return tsfEpsilon;
   }

   if (id == MASSFLOWSCALEFACTOR)
   {
      massFlowFactor = value;
      return massFlowFactor;
   }

   if (id == START_EPOCH || id == END_EPOCH)
   {
      throw ODEModelException("Thrust segment epoch can only be set through the Thrust History File");
   }

   return GmatBase::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id, Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for objecs in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::GetRealParameter(const Integer id, const Integer index) const
{
   return GmatBase::GetRealParameter(id, index);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id, const Integer row,
//                        const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param row Row index for parameters in arrays.
 * @param col Column index for parameters in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::GetRealParameter(const Integer id, const Integer row,
      const Integer col) const
{
   return GmatBase::GetRealParameter(id, row, col);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value, Integer index)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id    The integer ID for the parameter.
 * @param value The new parameter value.
 * @param index The array index in which the data is set.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real, or
 *         the parameter is not in a vector.
 */
//---------------------------------------------------------------------------
Real ThrustSegment::SetRealParameter(const Integer id, const Real value,
      const Integer index)
{
   return GmatBase::SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value, const Integer row,
//                        const Integer col
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id    The integer ID for the parameter.
 * @param value The new parameter value.
 * @param row   The row in which the data is set.
 * @param col   The column in which the data is set.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real, or
 *         the parameter is not in an array.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::SetRealParameter(const Integer id, const Real value,
      const Integer row, const Integer col)
{
   return GmatBase::SetRealParameter(id, value, row, col);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::GetRealParameter(const std::string &label,
      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::SetRealParameter(const std::string &label, const Real value,
      const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label, const Integer row,
//                        const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter from an array.
 *
 * @param label The (string) label for the parameter.
 * @param row Row index for parameters in arrays.
 * @param col Column index for parameters in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::GetRealParameter(const std::string &label,
      const Integer row, const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//                        const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter in an array.
 *
 * @param label The (string) label for the parameter.
 * @param value The new parameter value.
 * @param row   The row that receives the value.
 * @param col   The column that receives the value value.
 *
 * @return the parameter value at the end of this call.
 */
//------------------------------------------------------------------------------
Real ThrustSegment::SetRealParameter(const std::string &label, const Real value,
      const Integer row, const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw ab=n exception if
 *         there is no string association.
 */
//------------------------------------------------------------------------------
std::string ThrustSegment::GetStringParameter(const Integer id) const
{
   return GmatBase::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == MASSSOURCE)
   {
      // trim off braces:
      if (GmatStringUtil::IsEnclosedWithBraces(value))
      {
         std::string value1 = GmatStringUtil::RemoveEnclosingString(value, "{}");
         if (GmatStringUtil::Trim(value1) == "")
         {
            massSourceNames.clear();               // empty list of mass source names.
            return true;
         }
      }

      // verify input value:
      if (GmatStringUtil::IsValidIdentity(value) == false)
      {
         throw GmatBaseException("Error: The value \"" + value + "\" cannot accepted for " + GetName() + ".MassSource ");
         return false;
      }

      if (find(massSourceNames.begin(), massSourceNames.end(), value) == massSourceNames.end())
      {
         massSourceNames.push_back(value);
      }
      return true;
   }

   if (id == SOLVEFORS)
   {
      // trim off braces:
      if (GmatStringUtil::IsEnclosedWithBraces(value))
      {
         std::string value1 = GmatStringUtil::RemoveEnclosingString(value, "{}");
         if (GmatStringUtil::Trim(value1) == "")
         {
            solveFors.clear();               // empty list of solvefors.
            return true;
         }
      }

      // verify input value:
      if (GmatStringUtil::IsValidIdentity(value) == false)
      {
         throw ODEModelException("Error: The value \"" + value + "\" cannot be accepted for " + GetName() + ".SolveFors ");
         return false;
      }

      if (find(solveFors.begin(), solveFors.end(), value) == solveFors.end())
      {
         solveFors.push_back(value);
      }

      #ifdef DEBUG_TSF_SOLVEFOR
         MessageInterface::ShowMessage("ThrustSegment::SetStringParameter\n");
         for (UnsignedInt i = 0; i < solveFors.size(); ++i)
            MessageInterface::ShowMessage("ThrustSegment::SetStringParameter "
                  "Solve For   %s\n", solveFors[i].c_str());
      #endif

      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string ThrustSegment::GetStringParameter(const Integer id, const Integer index) const
{
   if (id == MASSSOURCE)
   {
      if ((index >= 0) && (index < massSource.size()))
         return massSourceNames[index];
      return "";
   }

   if (id == SOLVEFORS)
   {
      #ifdef DEBUG_TSF_SOLVEFOR
         MessageInterface::ShowMessage("ThrustSegment::GetStringParameter\n");
         for (UnsignedInt i = 0; i < solveFors.size(); ++i)
            MessageInterface::ShowMessage("ThrustSegment::GetStringParameter   "
                  "%s\n", solveFors[i].c_str());
      #endif
      if ((index >= 0) && (index < solveFors.size()))
         return solveFors[index];
      return "";
   }

   return GmatBase::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   if (index < 0)
      return false;     // Throw here?

   if (id == MASSSOURCE)
   {
      // verify input value:
      if (GmatStringUtil::IsValidIdentity(value) == false)
      {
         throw GmatBaseException("Error: The value \"" + value + "\" cannot accepted for " + GetName() + ".MassSource ");
         return false;
      }

      if (index < massSource.size())
      {
         massSourceNames[index] = value;
      }
      else if (index == massSource.size())
      {
         massSourceNames.push_back(value);
      }
      else
         return false;     // Throw here?

      return true;
   }

   if (id == SOLVEFORS)
   {
      #ifdef DEBUG_TSF_SOLVEFOR
	      MessageInterface::ShowMessage("ThrustSegment::SetStringParameter 3\n");
	      for (UnsignedInt i = 0; i < solveFors.size(); ++i)
	         MessageInterface::ShowMessage("ThrustSegment::SetStringParameter "
	               "Solve For  %s\n", solveFors[i].c_str());
      #endif
      if (index < solveFors.size())
      {
         solveFors[index] = value;
      }
      else if (index == solveFors.size())
      {
         solveFors.push_back(value);
      }
      else
         return false;     // Throw here?
      return true;
   }

   return GmatBase::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string ThrustSegment::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label,
//                                 const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string ThrustSegment::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& ThrustSegment::GetStringArrayParameter(const Integer id) const
{
   if (id == MASSSOURCE)
      return massSourceNames;

   if (id == SOLVEFORS)
   {
      #ifdef DEBUG_TSF_SOLVEFOR
	      MessageInterface::ShowMessage("ThrustSegment::GetStringArrayParameter\n");
         for (UnsignedInt i = 0; i < solveFors.size(); ++i)
            MessageInterface::ShowMessage("ThrustSegment::GetStringArray"
                  "Parameter   %s\n", solveFors[i].c_str());
      #endif
      return solveFors;
   }

   return GmatBase::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id,
//                                             const Integer index) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple StringArrays are supported.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& ThrustSegment::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return GmatBase::GetStringArrayParameter(id, index);
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& ThrustSegment::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label,
//                                             const Integer index) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param label The (string) label for the parameter.
 * @param index Which string array if more than one is supported.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& ThrustSegment::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param  id  The integer ID for the parameter.
 *
 * @return the boolean value for this parameter, or throw an exception if the
 *         parameter access in invalid.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::GetBooleanParameter(const Integer id) const
{
   if (id == TSF_MASSFLOW)
      return useMassAndThrustFactor;

   return GmatBase::GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new value.
 *
 * @return the boolean value for this parameter, or throw an exception if the
 *         parameter is invalid or not boolean.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == TSF_MASSFLOW)
   {
      useMassAndThrustFactor = value;
      return useMassAndThrustFactor;
   }

   return GmatBase::SetBooleanParameter(id, value);
}

//------------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::GetBooleanParameter(const Integer id,
      const Integer index) const
{
   return GmatBase::GetBooleanParameter(id, index);
}

//------------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value,
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new value for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::SetBooleanParameter(const Integer id, const bool value,
      const Integer index)
{
   return GmatBase::SetBooleanParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param label The (string) label for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::SetBooleanParameter(const std::string &label,
      const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool GetBooleanParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::GetBooleanParameter(const std::string &label,
      const Integer index) const
{
   return GetBooleanParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::SetBooleanParameter(const std::string &label,
      const bool value, const Integer index)
{
   return SetBooleanParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// void SetDataSegment(ThfDataSegment theData)
//------------------------------------------------------------------------------
/**
 * Adds the data from a history file into the segment
 *
 * @param theData The loaded data
 */
//------------------------------------------------------------------------------
void ThrustSegment::SetDataSegment(ThfDataSegment theData)
{
   segData = theData;
   if (segData.modelFlag.find("MassRate") != std::string::npos)
      depleteMass = true;
   else
      depleteMass = false;
}

//------------------------------------------------------------------------------
// bool ThrustSegment::DepletesMass()
//------------------------------------------------------------------------------
/**
 * Access method to check to see if the segment should deplete mass
 *
 * @return True if mass should be depleted, false if not.
 */
//------------------------------------------------------------------------------
bool ThrustSegment::DepletesMass()
{
   return depleteMass;
}

//------------------------------------------------------------------------------
// void GetScaleFactors(Real scaleFactors[2])
//------------------------------------------------------------------------------
/**
 * Retrieves the thrust scale factor and mass flow scale factor
 *
 * @param scaleFactors Array for the return data
 */
//------------------------------------------------------------------------------
void ThrustSegment::GetScaleFactors(Real scaleFactors[2])
{
   scaleFactors[0] = thrustScaleFactor;
   scaleFactors[1] = (useMassAndThrustFactor ?
         massFlowFactor * thrustScaleFactor : massFlowFactor);
}

//------------------------------------------------------------------------------
// Integer GetScaleFactorIndex()
//------------------------------------------------------------------------------
/**
 * Get the scale factor index so it can be linked to an STM row
 *
 * @return The index used to find the parameter in the STM
 */
 //------------------------------------------------------------------------------
Integer ThrustSegment::GetScaleFactorIndex()
{
   return tsfIndex;
}


//------------------------------------------------------------------------------
// void SetScaleFactorIndex(Integer index)
//------------------------------------------------------------------------------
/**
 * Set the scale factor index so it can be linked to an STM row
 *
 * @param index The index used to find the parameter in the STM
 */
 //------------------------------------------------------------------------------
void ThrustSegment::SetScaleFactorIndex(Integer index)
{
   tsfIndex = index;
}


//------------------------------------------------------------------------------
// bool ThrustSegment::IsEstimationParameterValid(const Integer item)
//------------------------------------------------------------------------------
/**
 * Checks to see if a parameter can be estimated
 *
 * @param item The ID for teh parameter, as known in the estimation system
 *
 * @return true if estimation is available, false if not
 */
//------------------------------------------------------------------------------
bool ThrustSegment::IsEstimationParameterValid(const Integer item)
{
   // Convert Estimation ID to object parameter ID
   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   #ifdef DEBUG_SOLVE_FOR_PARAMETERS
      MessageInterface::ShowMessage("Testing estimation capability for "
            "ID %d (%s)\n", id, GetParameterText(id).c_str());
   #endif

   if (id == THRUSTSCALEFACTOR)
      return true;

   return GmatBase::IsEstimationParameterValid(id);
}

//-------------------------------------------------------------------------
// Integer HasParameterCovariances(Integer parameterId)
//-------------------------------------------------------------------------
/**
* This function is used to verify whether a parameter (with ID specified by
* parameterId) has a covariance or not.
*
* @param parameterId      ID of a parameter
* @return                 size of covariance matrix associated with the parameter
*                         return -1 when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Integer ThrustSegment::HasParameterCovariances(Integer parameterId)
{
   if (parameterId == TSF_EPSILON)
      return 1;

   return GmatBase::HasParameterCovariances(parameterId);
}


//------------------------------------------------------------------------------
// Rmatrix* GetParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
* Get covariance of a given ThrustSegment's parameter
*
* @param paramId     The Id of a ThrustSegment's parameter
*
* @return            Covariance matrix of the parameter specified by the
*                    parameter Id
*/
//------------------------------------------------------------------------------
Rmatrix* ThrustSegment::GetParameterCovariances(Integer parameterId)
{
   if (isInitialized)
      return covariance.GetCovariance(parameterId);
   else
      throw GmatBaseException("Error: cannot get " + GetName() + " ThrustSegment's covariance when it is not initialized.\n");
   return NULL;
}


//------------------------------------------------------------------------------
// bool SetPrecisionTimeFlag(bool onOff)
//------------------------------------------------------------------------------
/**
* Set whether the thrust segment is using precision time or not
*
* @param onOff Flag indicating whether the thrust segment uses precision time
*
* @return Returns the value of the onOff flag passed in
*/
//------------------------------------------------------------------------------
bool ThrustSegment::SetPrecisionTimeFlag(bool onOff)
{
   hasPrecisionTime = onOff;
   segData.SetPrecisionTimeFlag(onOff);
   return hasPrecisionTime;
}
