//------------------------------------------------------------------------------
//                              ImplicitRungeKutta
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim
// Created: 2017.01.24
//
/**
* From original MATLAB prototype:
*  Author: S. Hughes.  steven.p.hughes@nasa.gov
*
* Explanation goes here.
*
*/
//------------------------------------------------------------------------------

#ifndef ImplicitRungeKutta_hpp
#define ImplicitRungeKutta_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "math.h"


class ImplicitRungeKutta
{
public:

   // class methods
   /// default constructor
   ImplicitRungeKutta();
   /// copy constructor
   ImplicitRungeKutta(const ImplicitRungeKutta &copy);
   /// operator =
   ImplicitRungeKutta& operator=(const ImplicitRungeKutta &copy);

   /// destructor
   virtual ~ImplicitRungeKutta();

   Rvector      GetQuadratureWeights();
   
   Integer      GetNumStagePointsPerMesh();

   Integer      GetNumStateStagePointsPerMesh();

   Integer      GetNumControlStagePointsPerMesh();

   Rvector      GetStageTimes();

   Rmatrix      GetParamDependArray();

   Rmatrix      GetFuncConstArray();

   Integer      GetNumDefectCons();

   Integer      GetNumPointsPerStep();

   void         GetDependencyChunk(Integer defectIdx, Integer pointIdx,
                                   Integer numVars,
                                   Rmatrix &aChunk, Rmatrix &bChunk);
   
   void         ComputeDependencies();

   void         ComputeAandB(Integer numVars, Rmatrix &aMat, Rmatrix &bMat);

   virtual void InitializeData() = 0;

   virtual void LoadButcherTable() = 0;
   
   virtual ImplicitRungeKutta* Clone() const = 0;


protected:
   /// Part of butcher array
   Rvector rhoVec;
   /// Part of butcher array
   Rmatrix sigmaMatrix;
   /// Part of butcher array
   Rvector betaVec;
   /// The number of defect constraints
   Integer numDefectCons;
   /// The non-dimensionalized times of the stages
   Rvector stageTimes;
   /// The "A" matrix chunk that describes dependency on opt.parameters
   Rmatrix paramDepArray;
   /// The "B" matrix that describes dependency on NLP functions
   Rmatrix funcConstArray;
   /// Number of points that have optimization parameters per step
   Integer numPointsPerStep;
   /// Number of stages between mesh points
   Integer numStagePointsPerMesh;
   /// Number stage points that have states
   Integer numStateStagePointsPerMesh;
   /// Number of stages that have control
   Integer numControlStagePointsPerMesh;
   /// The dependency pattern for the A matrix in Betts formulation
   Rmatrix patternAMat;
   /// The dependency pattern for the B matrix in Betts formulation
   Rmatrix patternBMat;
};

#endif // ImplicitRungeKutta_hpp
