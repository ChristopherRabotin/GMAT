//$Id: ErrorModel.hpp 1398 2015-01-07 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                 ErrorModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/01/07
//
/**
 * Definition for the ErrorModel class used to define an error model for a measurement model
 */
//------------------------------------------------------------------------------


#ifndef ErrorModel_hpp
#define ErrorModel_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"

class ESTIMATION_API ErrorModel : public GmatBase
//class GMAT_API ErrorModel : public GmatBase
{
public:
   ErrorModel(const std::string name);
   virtual ~ErrorModel();
   ErrorModel(const ErrorModel& em);
   ErrorModel& operator=(const ErrorModel& em);
   bool operator==(const ErrorModel& em);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();
   virtual bool Finalize();

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string& label) const;
   virtual Real         SetRealParameter(const std::string& label,
                                         const Real value);

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);

   //virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
   //                                  const std::string &name = "");

   virtual bool         IsEstimationParameterValid(const Integer id);
   virtual Integer      GetEstimationParameterSize(const Integer id);
   virtual Real*        GetEstimationParameterValue(const Integer id);

   virtual Integer      HasParameterCovariances(Integer parameterId);
   virtual Rmatrix*     GetParameterCovariances(Integer parameterId);

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Measurement type 
   std::string   measurementType;               // Its value to be ("Range_KM") "Range", SN_Range", "DSN_SeqRange", ("Doppler_RangeRate") "RangeRate", "DSN_TCP", ("TDRSDoppler_HZ") "SN_Doppler", etc
   ///// Measurement trip
   //Integer       measurementTrip;               // specify number of ways of a measurement. It would be 1 for one-way, 2 for two-ways, 3 for three-ways, and so on. It is 0 for all trips
   ///// Participant name list
   //StringArray   participantNameList;           // It contains a list of participant name
   /// Measurement noise sigma
   Real          noiseSigma;                    // specify noise sigma of a measurement
   ///// Noise Model
   //std::string   noiseModel;                    // specify noise model. It will be "RandomConstant" for Gausian noise model.
   /// Measurement bias
   Real          bias;                          // specify bias of a measurement
   /// Measurement bias sigma
   Real          biasSigma;                     // specify bias sigma of a measurement
   /// Solve-for parameters
   StringArray   solveforNames;                 // It contains a name list of solve-for parameters
   /// ErrorModel ID
   std::string   modelId;                       // Similar to the instanceName, but uses object ids instead of object names

   /// Class parameter ID enumeration
   enum
   {
      TYPE = GmatBaseParamCount,
      //TRIP,
      //STRAND,
      NOISE_SIGMA,
      //NOISE_MODEL,
      BIAS,
      BIAS_SIGMA,
      SOLVEFORS,
      MODEL_ID,
      ErrorModelParamCount
   };

   // Start with the parameter IDs and associates strings
   /// Strings associated with the ErrorModel parameters
   static const std::string
                PARAMETER_TEXT[ErrorModelParamCount - GmatBaseParamCount];
   /// Types of the ErrorModel parameters
   static const Gmat::ParameterType
                PARAMETER_TYPE[ErrorModelParamCount - GmatBaseParamCount];

private:
   StringArray    GetAllAvailableTypes();
//   std::string    CheckTypeDeprecation(const std::string datatype);
//   std::map<std::string, std::string> depTypeMap;
};

#endif /* ErrorModel_hpp */
