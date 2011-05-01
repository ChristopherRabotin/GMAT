//$Id: BatchEstimator.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         BatchEstimator
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
// Created: 2009/08/04
//
/**
 * Definition of the BatchEstimator class
 */
//------------------------------------------------------------------------------


#ifndef BatchEstimator_hpp
#define BatchEstimator_hpp

#include "Estimator.hpp"
#include "PropSetup.hpp"
#include "MeasurementManager.hpp"

#include "OwnedPlot.hpp"

/**
 * Implementation of a standard batch estimation state machine
 *
 * This class provides a batch estimation state machine that follows a typical
 * batch estimation process.  Derived classes override specific methods to
 * implement the math required for their algorithm.  Every derived estimator
 * must implement the Accumulate and Estimate methods.  The other methods called
 * in the finite state machine provide default implementation that can be
 * overridden as  needed.
 *
 * BatchEstimator is an abstract class; a derived class is needed to instantiate
 * a BatchEstimator
 */
class ESTIMATION_API BatchEstimator : public Estimator
{
public:
   BatchEstimator(const std::string &type, const std::string &name);
   virtual ~BatchEstimator();
   BatchEstimator(const BatchEstimator& est);
   BatchEstimator& operator=(const BatchEstimator& est);

   virtual bool         Initialize();
   virtual SolverState  AdvanceState();
   virtual bool         Finalize();

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

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
   virtual const StringArray& GetPropertyEnumStrings(const Integer id) const;

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

protected:
   /// Time system used to specify the estimation epoch
   std::string             estEpochFormat;
   /// The estimation epoch.  "FromParticipants" means use the spacecraft epoch.
   std::string             estEpoch;
   /// RMS residual value from the previous pass through the data
   Real                    oldResidualRMS;
   /// RMS residual value froooom the current pass through the data
   Real                    newResidualRMS;
   /// Flag set when an a priori estimate is available
   bool                    useApriori;
   /// The most recently computed state vector changes
   RealArray               dx;
   /// The weighting matrix used when accumulating data
   Rmatrix                 weights;
   /// Flag used to indicate propagation to estimation epoch is executing
   bool                    advanceToEstimationEpoch;
   /// Flag indicating convergence
   bool                    converged;
   /// Buffer of the participants for the outer batch loop
   ObjectArray             outerLoopBuffer;

   /// Parameter IDs for the BatchEstimators
   enum
   {
      ESTIMATION_EPOCH_FORMAT = EstimatorParamCount,
      ESTIMATION_EPOCH,
      BatchEstimatorParamCount,
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string
                           PARAMETER_TEXT[BatchEstimatorParamCount -
                                              EstimatorParamCount];
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[BatchEstimatorParamCount -
                                              EstimatorParamCount];

   virtual void            CompleteInitialization();
   virtual void            FindTimeStep();
   virtual void            CalculateData();
   virtual void            ProcessEvent();
   virtual void            CheckCompletion();
   virtual void            RunComplete();

   // Abstract methods defined in derived classes
   /// Abstract method that implements accumulation in derived classes
   virtual void            Accumulate() = 0;
   /// Abstract method that performs the estimation in derived classes
   virtual void            Estimate() = 0;

   virtual bool            TestForConvergence(std::string &reason);
   virtual void            WriteToTextFile(Solver::SolverState state =
                                              Solver::UNDEFINED_STATE);

   // progress string for reporting
   virtual std::string    GetProgressString();
};

#endif /* BatchEstimator_hpp */
