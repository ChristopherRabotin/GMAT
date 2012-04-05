//$Id: Receiver.cpp 47 2010-03-31$
//------------------------------------------------------------------------------
//                             Receiver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen
// Created: 2010/03/31 by Tuan Dang Nguyen (GSFC-NASA)
//
/**
 * Implementation for the Receiver class
 */
//------------------------------------------------------------------------------

#ifndef Receiver_hpp
#define Receiver_hpp

#include "estimation_defs.hpp"
#include "RFHardware.hpp"

class ESTIMATION_API Receiver: public RFHardware
{
public:
	Receiver(const std::string &name);
	virtual ~Receiver();
	Receiver(const Receiver& recei);
	Receiver& operator=(const Receiver& recei);

	virtual GmatBase* Clone() const;
	virtual void      Copy(const GmatBase* recei);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   // These are the parameter accessors.  The commented out versions may be
   // needed if derived classes implement them in order to prevent compiler
   // confusion.
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
//   virtual Real         GetRealParameter(const Integer id,
//                                      const Integer index) const;
//   virtual Real         GetRealParameter(const Integer id, const Integer row,
//                                      const Integer col) const;
//   virtual Real         SetRealParameter(const Integer id,
//                                      const Real value,
//                                      const Integer index);
//   virtual Real         SetRealParameter(const Integer id, const Real value,
//                                      const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
//   virtual Real         GetRealParameter(const std::string &label,
//                                      const Integer index) const;
//   virtual Real         SetRealParameter(const std::string &label,
//                                      const Real value,
//                                      const Integer index);
//   virtual Real         GetRealParameter(const std::string &label,
//                                      const Integer row,
//                                      const Integer col) const;
//   virtual Real         SetRealParameter(const std::string &label,
//                                      const Real value, const Integer row,
//                                      const Integer col);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                         const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                         const std::string &value);

   virtual bool         Initialize();

   virtual Real			GetDelay(Integer whichOne=0);
   virtual bool 			SetDelay(Real delay, Integer whichOne=0);
   virtual bool 			IsFeasible(Integer whichOne=0);
   virtual Integer 		GetSignalCount();
   virtual bool			IsTransmitted(Integer whichOne=0);
   virtual Signal*		GetSignal(Integer whichOne=0);
   virtual bool 			SetSignal(Signal* s,Integer whichOne=0);

   DEFAULT_TO_NO_REFOBJECTS

protected:
   std::string	frequencyModel;
   Real			centerFrequency;
   Real 			bandwidth;

   /// Published parameters for the RF hardware
   enum
   {
      FREQUENCY_MODEL = RFHardwareParamCount,
      CENTER_FREQUENCY,
      BANDWIDTH,
      ReceiverParamCount,
   };

   static const std::string
      PARAMETER_TEXT[ReceiverParamCount - RFHardwareParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[ReceiverParamCount - RFHardwareParamCount];

};

#endif /* Receiver_hpp */
