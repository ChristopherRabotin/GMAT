//$Id: SequentialEstimator.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         SequentialEstimator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/09/03
//
/**
 * Base class for sequential estimation
 */
//------------------------------------------------------------------------------


#ifndef SequentialEstimator_hpp
#define SequentialEstimator_hpp

#include "Estimator.hpp"


/**
 * Provides core functionality used in sequential estimation.
 *
 * This class provides the data structures and interfaces used when performing
 * sequential estimation.  The class includes an implementation of a finite
 * state machine that represents the typical state transitions encountered when
 * performing sequential estimation in, for example, an extended Kalman filter.
 */
class ESTIMATION_API SequentialEstimator: public Estimator
{
public:
   SequentialEstimator(const std::string &type, const std::string &name);
   virtual ~SequentialEstimator();
   SequentialEstimator(const SequentialEstimator &se);
   SequentialEstimator&    operator=(const SequentialEstimator &se);

   virtual bool         Initialize();
   virtual SolverState  AdvanceState();
   virtual bool         Finalize();

   // methods overridden from GmatBase
   // These methods are commented out so that they can be easily implemented if
   // the SequentialEstimator class needs parameter support.

//   virtual std::string  GetParameterText(const Integer id) const;
//   virtual std::string  GetParameterUnit(const Integer id) const;
//   virtual Integer      GetParameterID(const std::string &str) const;
//   virtual Gmat::ParameterType
//                        GetParameterType(const Integer id) const;
//   virtual std::string  GetParameterTypeString(const Integer id) const;
//
//   virtual std::string  GetStringParameter(const Integer id) const;
//   virtual bool         SetStringParameter(const Integer id,
//                                           const std::string &value);
//   virtual std::string  GetStringParameter(const Integer id,
//                                           const Integer index) const;
//   virtual bool         SetStringParameter(const Integer id,
//                                           const std::string &value,
//                                           const Integer index);
//   virtual std::string  GetStringParameter(const std::string &label) const;
//   virtual bool         SetStringParameter(const std::string &label,
//                                           const std::string &value);
//   virtual std::string  GetStringParameter(const std::string &label,
//                                           const Integer index) const;
//   virtual bool         SetStringParameter(const std::string &label,
//                                           const std::string &value,
//                                           const Integer index);

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

protected:
   /// Changes in the state vector
   Rvector                 dx;
   /// Array used to track the one-sigma deviation for each measurement point
   RealArray               sigma;

   /// The measurement error covariance
   Covariance              *measCovariance;

   // Methods used in the finite state machine
   virtual void            CompleteInitialization();
   virtual void            FindTimeStep();
   virtual void            CalculateData();
   virtual void            ProcessEvent();
   virtual void            CheckCompletion();
   virtual void            RunComplete();

   /**
    *  Abstract method that is overridden in derived classes to perform
    *  sequential estimation.
    */
   virtual void            Estimate() = 0;

   virtual void            PrepareForStep();

//   virtual bool            TestForConvergence(std::string &reason);
   virtual void            WriteToTextFile(Solver::SolverState state =
                                                Solver::UNDEFINED_STATE);

   virtual std::string     GetProgressString();
};

#endif /* SequentialEstimator_hpp */
