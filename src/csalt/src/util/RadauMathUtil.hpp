//------------------------------------------------------------------------------
//                         RadauMathUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim
// Created: 2015/07/08
//
// NOTE: This is a static utility class.  The constructors, destructor, and
// operator= are not implemented.  No instances of this class can be
// instantiated.
//
/**
 */
//------------------------------------------------------------------------------
#ifndef RadauMathUtil_hpp
#define RadauMathUtil_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "SparseMatrixUtil.hpp" // for sparse matrix

/*
 * RadauMathUtil class
*/

class RadauMathUtil
{
public:
   //  the methods using Rvector
   /// 'collocD.m'; this algorithm does not assume LGR nodes.
   static RSMatrix GetLagrangeDiffMatrix(const Rvector *lgrNodes);     

   /// 'lgrPS.m'; this algorithm assumes multiple sets of LGR nodes on [-1,1] 
   static bool     ComputeMultiSegmentLGRNodes(const Rvector *segmentPoints,
                                               const IntegerArray *numNodesVec,
                                               Rvector &lgrNodes,
                                               Rvector &lgrWeightVec,
                                               RSMatrix &lagDiffMat);

   /// 'lgrnodes.m'; this algorithm assumes single set of LGR nodes on [-1,1] 
   static void     ComputeSingleSegLGRNodes(Integer numNodes,
                                            Rvector &lgrNodes,
                                            Rvector &lgrWeightVec);
   
private:
   /// private constructors, destructor, operator=   *** UNIMPLEMENTED ***
   RadauMathUtil();
   RadauMathUtil(const RadauMathUtil &copy);
   RadauMathUtil& operator=(const RadauMathUtil &copy);
   virtual ~RadauMathUtil();
   
};

#endif // RadauMathUtil_hpp

