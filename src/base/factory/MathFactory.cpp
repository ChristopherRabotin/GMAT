//$Header$
//------------------------------------------------------------------------------
//                              MathFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2006/04/05
//
/**
 * Implementation code for the MathFactory class, responsible for creating 
 * MathNode objects.
 */
//------------------------------------------------------------------------------

#include "MathFactory.hpp"
#include "StringUtil.hpp"          // for Capitalize()
#include "MessageInterface.hpp"

// include list of MathNode classes here
#include "MathElement.hpp"
#include "Abs.hpp"
#include "Acos.hpp"
#include "Asin.hpp"
#include "Atan.hpp"
#include "Atan2.hpp"
#include "Add.hpp"
#include "Cos.hpp"
#include "DegToRad.hpp"
#include "Determinant.hpp"
#include "Divide.hpp"
#include "Exp.hpp"
#include "Inverse.hpp"
#include "Log.hpp"
#include "Log10.hpp"
#include "Multiply.hpp"
#include "Negate.hpp"
#include "Norm.hpp"
#include "Power.hpp"
#include "RadToDeg.hpp"
#include "Sin.hpp"
#include "Sqrt.hpp"
#include "Subtract.hpp"
#include "Tan.hpp"
#include "Transpose.hpp"

//#define DEBUG_MATH_FACTORY 1

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateMathNode(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested MathNode class 
 *
 * @param <ofType>   the MathNode object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
MathNode* MathFactory::CreateMathNode(const std::string &ofType,
                                      const std::string &withName)
{
   MathNode *mathNode = NULL;

   // The FIRST letter of function name can be either lower or upper case,
   // so capitalize the first letter of the type before checking.
   // eg) cos, Cos, add, Add.
   
   std::string newType = GmatStringUtil::Capitalize(ofType);

   #if DEBUG_MATH_FACTORY
   MessageInterface::ShowMessage
      ("MathFactory::CreateMathNode() ofType=%s, newType=%s, withName=%s\n",
       ofType.c_str(), newType.c_str(), withName.c_str());
   #endif
   
   // Leaf node
   if (ofType == "MathElement")
      mathNode = new MathElement(ofType, withName);
   
   // Simple math operations
   else if (newType == "Add")         // Add(x,y) or x+y
      mathNode = new Add(withName);
   else if (newType == "Subtract")    // Subtract(x,y) or x-y
      mathNode = new Subtract(withName);
   else if (newType == "Multiply")    // Multiply(x,y) or x*y
      mathNode = new Multiply(withName);
   else if (newType == "Divide")      // Divide(x,y) or x/y
      mathNode = new Divide(withName);
   else if (newType == "Negate")      // Negate(x)
      mathNode = new Negate(withName);
   else if (newType == "Sqrt")        // Sqrt(x)
      mathNode = new Sqrt(withName);
   else if (newType == "Abs")         // Abs(x)
      mathNode = new Abs(withName);
   
   // Power, Log functions
   else if (newType == "Power")       // power(x,y) or x^y
      mathNode = new Power(withName);
   else if (newType == "Exp")         // exp(x)
      mathNode = new Exp(withName);
   else if (newType == "Log")         // log(x)
      mathNode = new Log(withName);
   else if (newType == "Log10")       // log10(x)
      mathNode = new Log10(withName);
   
   // Matrix functions
   else if (newType == "Transpose")   // transpose(m) or m'
      mathNode = new Transpose(withName);
   else if (newType == "Det")         // det(m)
      mathNode = new Determinant(withName);
   else if (newType == "Inv")         // inv(m)
      mathNode = new Inverse(withName);
   else if (newType == "Norm")        // norm(m)
      mathNode = new Norm(withName);
   
   // Trigonometric functions
   else if (newType == "Sin")         // sin(x)
      mathNode = new Sin(withName);
   else if (newType == "Cos")         // cos(x)
      mathNode = new Cos(withName);
   else if (newType == "Tan")         // tan(x)
      mathNode = new Tan(withName);
   else if (newType == "Asin")        // asin(x)
      mathNode = new Asin(withName);
   else if (newType == "Acos")        // acos(x)
      mathNode = new Acos(withName);
   else if (newType == "Atan")        // atan(x)
      mathNode = new Atan(withName);
   else if (newType == "Atan2")       // atan2(y,x)
      mathNode = new Atan2(withName);
   
   // Unit conversion functions
   else if (newType == "DegToRad")
      mathNode = new DegToRad(withName);
   else if (newType == "RadToDeg")
      mathNode = new RadToDeg(withName);

   
   return mathNode;
}


//------------------------------------------------------------------------------
//  MathFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MathFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
MathFactory::MathFactory()
   : Factory(Gmat::MATH_NODE)
{
   isCaseSensitive = false;
   
   if (creatables.empty())
      BuildCreatables();
}


//------------------------------------------------------------------------------
//  MathFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MathFactory.
 *
 * @param <createList> list of creatable MathNode objects
 */
//------------------------------------------------------------------------------
MathFactory::MathFactory(StringArray createList)
   : Factory(createList, Gmat::MATH_NODE)
{
   isCaseSensitive = true;
}


//------------------------------------------------------------------------------
//  MathFactory(const MathFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MathFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
MathFactory::MathFactory(const MathFactory &fact)
   : Factory(fact)
{
   isCaseSensitive = true;
   
   if (creatables.empty())
      BuildCreatables();
}


//------------------------------------------------------------------------------
//  MathFactory& operator= (const MathFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the MathFactory base class.
 *
 * @param <fact> the MathFactory object whose data to assign to "this" factory.
 *
 * @return "this" MathFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
MathFactory& MathFactory::operator=(const MathFactory &fact)
{
   Factory::operator=(fact);
   isCaseSensitive = true;
   return *this;
}


//------------------------------------------------------------------------------
// ~MathFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the MathFactory class.
 */
//------------------------------------------------------------------------------
MathFactory::~MathFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
/**
 * Fills in creatable objects
 */
//------------------------------------------------------------------------------
void MathFactory::BuildCreatables()
{
   // The FIRST letter of function name can be either lower or upper case.
   
   // Math element
   creatables.push_back("MathElement");
   
   // Simple math operations
   creatables.push_back("Negate");
   creatables.push_back("Add");
   creatables.push_back("Subtract");
   creatables.push_back("Multiply");
   creatables.push_back("Divide");
   creatables.push_back("sqrt");
   creatables.push_back("abs");
   
   // Power, Log functions
   creatables.push_back("Power");
   creatables.push_back("exp");
   creatables.push_back("log");
   creatables.push_back("log10");
   
   // Matrix functions
   creatables.push_back("transpose");
   creatables.push_back("det");
   creatables.push_back("inv");
   creatables.push_back("norm");
   
   // Trigonometric functions
   creatables.push_back("sin");
   creatables.push_back("cos");
   creatables.push_back("tan");
   creatables.push_back("asin");
   creatables.push_back("acos");
   creatables.push_back("atan");
   creatables.push_back("atan2");
   
   // Unit conversion functions
   creatables.push_back("degToRad");  
   creatables.push_back("radToDeg");
   
}

