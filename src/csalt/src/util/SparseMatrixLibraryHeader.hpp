//------------------------------------------------------------------------------
//                       SparseMatrixLibraryHeader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim
// Created: 2015
//
/**
 *  This file is used to set up use of Boost libraries
 */
//------------------------------------------------------------------------------

#include "SparseMatrixUtil.hpp"

// note that BOOST compressed matrix has a better performance
//           than coordinate matrix type.
#define _use_BOOST_Compressed_Matrix 

//using SparseMatrixUtil;

// using Iterators are not recommended
// because they are BOOST dependent!
#ifdef _use_BOOST_Compressed_Matrix 
////using BOOST_Compressed_Matrix::RSMatrix;
//using BOOST_Compressed_Matrix::ConstIterator1;
//using BOOST_Compressed_Matrix::ConstIterator2;
//using BOOST_Compressed_Matrix::Iterator1;
//using BOOST_Compressed_Matrix::Iterator2;
////using BOOST_Compressed_Matrix::SparseMatrixUtil;
#endif

