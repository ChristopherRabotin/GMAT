//$Id: EstimationStateManager.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         EstimationStateManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/08/05
//
/**
 * Definition for the estimation state manager.
 */
//------------------------------------------------------------------------------


#ifndef EstimationStateManager_hpp
#define EstimationStateManager_hpp

#include "GmatBase.hpp"
#include "StateManager.hpp"
#include "MeasurementManager.hpp"
#include "PropagationStateManager.hpp"
#include "Rmatrix.hpp"

/**
 * Mediator between objects, the estimators, and observations and measurements
 *
 * The estimation state manager is responsible for constructing and filling the
 * estimation state vector, state transition matrix, and covariance matrix used
 * in the estimation process.
 */
class ESTIMATION_API EstimationStateManager : public StateManager
{
public:
   EstimationStateManager(Integer size = 0);
   virtual ~EstimationStateManager();
   EstimationStateManager(const EstimationStateManager &esm);
   EstimationStateManager& operator=(const EstimationStateManager &esm);

   // Methods used to define the estimation state vector
   virtual const StringArray& GetObjectList(std::string ofType = "");
   virtual bool               SetObject(GmatBase* obj);
   virtual bool               SetProperty(std::string prop);
   virtual bool               SetProperty(std::string, Integer);
   virtual bool               SetProperty(std::string, GmatBase*);
   virtual bool               SetConsider(std::string prop);
   virtual bool               SetConsider(std::string, Integer);
   virtual bool               SetConsider(std::string, GmatBase*);
   virtual void               SetParticipantList(StringArray &p);

   virtual bool               BuildState();
   virtual bool               MapObjectsToVector();
   virtual bool               MapVectorToObjects();
   virtual bool               MapObjectsToSTM();
   virtual bool               MapSTMToObjects();
   virtual bool               MapObjectsToCovariances();
   virtual bool               MapCovariancesToObjects();

   virtual Rmatrix*           GetSTM();
   virtual Covariance*        GetCovariance();

   virtual void               SetMeasurementManager(MeasurementManager *mm);

   virtual void               BufferObjects(ObjectArray *buffer = NULL);
   virtual void               RestoreObjects(ObjectArray *fromBuffer = NULL);

//   void                       SolveFor(std::string whichOne = "");
//   void                       Consider(std::string whichOne = "");


protected:
   // Attributes

   /// Clones of the objects that supply estimation data, used to restore later
   ObjectArray                estimationObjectClones;

   // Estimation State Pieces
   /// The text identities of the SolveFor parameters
   StringArray                solveForNames;
   /// The names of the SolveFor objects
   StringArray                solveForObjectNames;
   /// The objects providing solve for data.  Repetition is allowed
   ObjectArray                solveForObjects;
   /// Strings used to set property IDs of the solve for parameters
   StringArray                solveForIDNames;
   /// Property IDs of the solve for parameters
   IntegerArray               solveForIDs;

   /// The text identities of the consider parameters
   StringArray                considerNames;
   /// Names of the objects providing consider data.  Repetition is allowed
   StringArray                considerObjectNames;
   /// The objects providing consider data.  Repetition is allowed
   ObjectArray                considerObjects;
   /// Strings used to set property IDs of the consider parameters
   StringArray                considerIDNames;
   /// Property IDs of the consider parameters
   IntegerArray               considerIDs;

   /// Names of the participants, so ones not in other lists are managed
   StringArray                participantNames;

   // STM Pieces
   /// The total state transition matrix used in the estimation
   Rmatrix                    stm;
   /// Mapping used to associate STM elements with objects
   ObjectMap                  stmMap;
   /// Number of columns in the STM matrix
   Integer                    stmColCount;
//   /// Number of rows in the STM
//   Integer                    stmRowCount;

   // Covariance pieces
   /// The covariance matrix
   Covariance                 covariance;
   /// Object associations for the covariances
   ObjectMap                  covMap;
   /// Number of columns in the covariance matrix
   Integer                    covColCount;
//   /// Number of rows in the covariance matrix
//   Integer                    covRowCount;

   // Propagation State Pieces
   /// PSM manages components that evolve over time via numerical integration
   PropagationStateManager    *psm;
   /// For convenience, the ESM has direct access to the PSM's state vector
   GmatState                  *propagationState;

   /// Pointer to the measurement manager used with this estimator
   MeasurementManager         *measMan;

   /// A buffer used for string parsing
   StringArray                chunks;

   // Protected methods
   void                       DecomposeParameter(std::string &param,
                                                 Integer howMany = -1);

   Integer                    SortVector();

};

#endif /* EstimationStateManager_hpp */
