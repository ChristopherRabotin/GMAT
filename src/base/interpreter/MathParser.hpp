//$Header$
//------------------------------------------------------------------------------
//                               ScriptInterpreter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2005/10/31
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS task
// 124.
//
/**
 * Class definition for the MathParser
 * 
 * The MathParser class takes a line of script that evaluates to inline math,
 * and breaks that line apart into its component elements using a recursive 
 * descent algorithm.  The resulting representation is stored in a binary tree 
 * structure, which is calculated, depth first, when the expression needs to be
 * evaluated during execution of a script.
 */
//------------------------------------------------------------------------------


#ifndef MATHPARSER_HPP_
#define MATHPARSER_HPP_


#include "gmatdefs.hpp"


class MathParser
{
public:
	MathParser();
	virtual ~MathParser();
//   MathParser(const MathParser& mp);
//   MathParser&       operator=(const MathParser& mp);
//   
//   void              Build(std::string expression);
//   Real              Evaluate();
//   std::string       ReconstructExpression();
};

#endif /*MATHPARSER_HPP_*/
