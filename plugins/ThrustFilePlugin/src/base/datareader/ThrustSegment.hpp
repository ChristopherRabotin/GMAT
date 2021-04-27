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

#ifndef ThrustSegment_hpp
#define ThrustSegment_hpp

#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "FuelTank.hpp"
#include "ThfDataSegment.hpp"

/**
 * Container for thrust history file segment data
 */
class ThrustSegment : public GmatBase
{
public:
   ThrustSegment(const std::string &name);
   virtual ~ThrustSegment();
   ThrustSegment(const ThrustSegment &ts);
   ThrustSegment& operator=(const ThrustSegment &ts);

   virtual GmatBase* Clone() const;

   // Ref. object access methods - overridden from GmatBase
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetRefObject(GmatBase *obj,
                                     const UnsignedInt type,
                                     const std::string &name = "");
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
//   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   // Reals
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         GetRealParameter(const Integer id, const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         SetRealParameter(const Integer id, const Real value,
                                         const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value, const Integer row,
                                         const Integer col);

   // Strings
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   // Bools
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const Integer id,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value,
                                            const Integer index);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value,
                                            const Integer index);

   void                 SetDataSegment(ThfDataSegment theData);
   bool                 DepletesMass();

   void                 GetScaleFactors(Real scaleFactors[2]);
   Integer              GetScaleFactorIndex();
   void                 SetScaleFactorIndex(Integer index);

   // May need some of these:
   virtual bool            IsEstimationParameterValid(const Integer id);

   virtual Integer         HasParameterCovariances(Integer parameterId);
   virtual Rmatrix*        GetParameterCovariances(Integer parameterId);

   virtual bool            SetPrecisionTimeFlag(bool onOff);

   DEFAULT_TO_NO_CLONES

   /// The data from the file
   ThfDataSegment segData;

   /// Sources of mass flow names
   StringArray massSourceNames;
   /// Sources of mass flow
   std::vector<FuelTank *> massSource;

protected:
   // Object fields
   /// The thrust scale factor
   Real thrustScaleFactor;
   /// The thrust scale factor epsilon
   Real tsfEpsilon;
   /// Standard deviation for the TSF
   Real tsfSigma;
   /// Mass flow flag
   bool depleteMass;
   /// Apply both scale factors to mass flow
   bool useMassAndThrustFactor;
   /// Mass flow scale factor
   Real massFlowFactor;
   /// STM index
   Integer tsfIndex;
   /// Solve for parameters
   StringArray solveFors;

   /// The object array used in GetRefObjectArray()
   ObjectArray objectArray;

   /// Parameter IDs
   enum
   {
      THRUSTSCALEFACTOR = GmatBaseParamCount,
      TSF_SIGMA,
      TSF_MASSFLOW,
      MASSFLOWSCALEFACTOR,
      MASSSOURCE,
      SOLVEFORS,
      TSF_EPSILON,
      START_EPOCH,
      END_EPOCH,
      ThrustSegmentParamCount,
   };

   /// Interface parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[ThrustSegmentParamCount - GmatBaseParamCount];
   /// Interface parameter labels
   static const std::string PARAMETER_LABEL[ThrustSegmentParamCount - GmatBaseParamCount];
};

#endif /* ThrustSegment_hpp */
