//$Id: Estimator.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         Estimator
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
// Created: 2009/08/03
//
/**
 * Definition of the Estimator base class
 */
//------------------------------------------------------------------------------


#ifndef Estimator_hpp
#define Estimator_hpp

#include "Solver.hpp"
#include "MeasurementManager.hpp"
#include "EstimationStateManager.hpp"
#include "PropSetup.hpp"

/**
 * The base class used for GMAT's estimators
 */
class ESTIMATION_API Estimator : public Solver
{
public:
   Estimator(const std::string &type, const std::string &name);
   virtual ~Estimator() = 0;
   Estimator(const Estimator& est);
   Estimator& operator=(const Estimator& est);

   virtual bool         Initialize();
   virtual void         CompleteInitialization();
   virtual bool         Finalize();

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);

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

   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id,
                                         const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label,
                                          const std::string &value);
   virtual Gmat::ObjectType
                        GetPropertyObjectType(const Integer id) const;

   // Access methods for the reference objects
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObjectName(const Gmat::ObjectType type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   Real                 GetTimeStep();

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   virtual void         UpdateCurrentEpoch(GmatEpoch newEpoch);
   virtual GmatEpoch    GetCurrentEpoch();

   PropSetup*           GetPropagator();
   MeasurementManager*  GetMeasurementManager();
   EstimationStateManager*
                        GetEstimationStateManager();

   bool                 ResetState();

   virtual bool HasLocalClones();
   virtual void UpdateClonedObject(GmatBase *obj);

protected:
   /// Names of the measurement objects used in the estimation
   StringArray             measurementNames;
   /// The solve for parameters in the estimation problem
   StringArray             solveForStrings;
   /// The consider parameters in the estimation problem
   StringArray             considerStrings;
   /// Tolerance measure applied to RMS state change to test for convergence
   Real                    absoluteTolerance;
   /// Tolerance measure applied to RMS residual change to test for convergence
   Real                    relativeTolerance;
   /// Name of the PropSetup used to evolve the system
   std::string             propagatorName;
   /// Flag used to check for a state load from the objects prior to propagation
   bool                    resetState;


   //---------------------------------------------------------------------------
   // Internal Data
   //---------------------------------------------------------------------------

   /**
    *  The timestep that gets returned for the next propagation
    *
    *  timeStep will be the same as estimationStep when the state machine is in
    *  the PROPAGATING state.  When in the LOCATING state, timeStep is the time
    *  from the base epoch to the next attempt at finding the event that is
    *  being located.
    */
   Real                    timeStep;
   /// Local storage element for ref object names
   StringArray             refObjectList;

   /// The estimator's measurement manager
   MeasurementManager      measManager;
   /// The estimator's state manager
   EstimationStateManager  esm;
   /// The propagator configured for simulation
   PropSetup               *propagator;

   /// Epoch of the current estimation state
   GmatEpoch               estimationEpoch;
   /// Currrrent simulation epoch
   GmatEpoch               currentEpoch;
   /// The next epoch desired from propagation
   GmatEpoch               nextMeasurementEpoch;

   /// The measurement derivatives at the measurement epoch, \tilde H
   std::vector<RealArray>  hTilde;
   /// The accumulated measurement data (transformed to the estimation epoch)
   std::vector<RealArray>  hAccum;
   /// The indices for the MeasurementModels with observations at current epoch
   IntegerArray            modelsToAccess;

   /// The estimation state transition matrix
   Rmatrix                 *stm;
   /// The estimation state covariance matrix
   Covariance              *stateCovariance;
   /// The estimated state
   GmatState               *estimationState;
   /// Size of the estimation state vector
   UnsignedInt             stateSize;

   /// The information matrix, $\Lambda$
   Rmatrix                 information;
   /// The residual vector, N in Tapley, Schutz and Born
   Rvector                 residuals;
   /// The a priori state, \bar x_0 in Tapley, Schutz and Born
   Rvector                 x0bar;
   /// The O-C value for each observation
   RealArray               measurementResiduals;
   /// The observation epochs
   RealArray               measurementEpochs;
   /// The observation ID for the residual
   IntegerArray            measurementResidualID;

   /// Flag set to show all residuals during processing
   bool                    showAllResiduals;
   /// Flag used to only display a specific set of residuals
   bool                    showSpecificResiduals;
   /// Toggle for displayign error bars
   bool                    showErrorBars;
   /// List of the residuals plot names
   StringArray             addedPlots;

   /// The owned residuals plots used by this estimator
   std::vector<OwnedPlot*> residualPlots;
   /// The data used to generate error bars on the residuals plots
   std::vector<RealArray*> hiLowData;

   /// Vector of the current active Event objects
   ObjectArray             activeEvents;
   /// Flag indicating is an Event is currently being located
   bool                    locatingEvent;

   /// Parameters associated with the Estimators
   enum
   {
      MEASUREMENTS  = SolverParamCount,
      SOLVEFORS,
      ABSOLUTETOLERANCE,
      RELATIVETOLERANCE,
      PROPAGATOR,
      SHOW_RESIDUALS,
      ADD_RESIDUAL_PLOT,
      EstimatorParamCount
   };

   /// String identifiers for the parameters
   static const std::string    PARAMETER_TEXT[EstimatorParamCount -
                                              SolverParamCount];
   /// Types of the parameters
   static const Gmat::ParameterType
                               PARAMETER_TYPE[EstimatorParamCount -
                                              SolverParamCount];

   virtual bool            TestForConvergence(std::string &reason);

   Real                    ConvertToRealEpoch(const std::string &theEpoch,
                                              const std::string &theFormat);

   virtual void            BuildResidualPlot(const std::string &plotName,
                              const StringArray &measurementNames);
   virtual void            PlotResiduals();
   virtual void            EnhancePlot();


   virtual Integer         SetSolverResults(Real*, const std::string&,
                                            const std::string&);
   virtual void            SetResultValue(Integer, Real, const std::string&);
};

#endif /* Estimator_hpp */
