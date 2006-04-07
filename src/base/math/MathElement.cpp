//$Header$
//------------------------------------------------------------------------------
//                                   MathElement
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Waka Waktola
// Created: 2006/04/04
//
/**
 * Defines the Math elements class for Math in scripts.
 */
//------------------------------------------------------------------------------

#include "MathElement.hpp"

#if DEBUG_MATHELEMENT
#include "MessageInterface.hpp"
#endif


//------------------------------------------------------------------------------
//  MathElement(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the MathElement object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <nomme>   Name for the object
 */
//------------------------------------------------------------------------------
MathElement::MathElement(const std::string &typeStr, const std::string &nomme) :
             MathNode      (typeStr, nomme),
             refObject     (NULL),
             refObjectName ("")
{
}

//------------------------------------------------------------------------------
//  ~MathElement(void)
//------------------------------------------------------------------------------
/**
 * Destroys the MathElement object (destructor).
 */
//------------------------------------------------------------------------------
MathElement::~MathElement()
{
}

//------------------------------------------------------------------------------
//  MathElement(const MathElement &me)
//------------------------------------------------------------------------------
/**
 * Constructs the MathElement object (copy constructor).
 * 
 * @param <me> Object that is copied
 */
//------------------------------------------------------------------------------
MathElement::MathElement(const MathElement &me) :
             MathNode      (me),
             refObject     (me.refObject),
             refObjectName (me.refObjectName)
{

}

//------------------------------------------------------------------------------
//  MathElement& operator=(const MathElement &me)
//------------------------------------------------------------------------------
/**
 * Sets one MathElement object to match another (assignment operator).
 * 
 * @param <me> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
MathElement& MathElement::operator=(const MathElement &me)
{
    if (this == &me)
        return *this;
        
    GmatBase::operator=(me);

    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MathElement.
 *
 * @return clone of the MathElement.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MathElement::Clone(void) const
{
   return (new MathElement(*this));
}

//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return reference object pointer.
 */
 //---------------------------------------------------------------------------
GmatBase* MathElement::GetRefObject(const Gmat::ObjectType type, 
                                    const std::string &name)
{
   switch (type)
   {
      case Gmat::MATH_NODE:
         return refObject;
         
      default:
         break;
   }

   return GmatBase::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type, 
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference object pointer.
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return true if successful false otherwise.
 */
 //---------------------------------------------------------------------------
bool MathElement::SetRefObject(GmatBase *obj, const Gmat::ObjectType type, 
                               const std::string &name)
{
   switch (type)
   {
      case Gmat::MATH_NODE:
         refObject = (MathNode*) obj;
         refObjectName = name;
         
      default:
         break;
   }	
   
   return GmatBase::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * This method returns a name of the referenced objects.
 *
 * @return the name of the object of the requested type.
 */
 //------------------------------------------------------------------------------
std::string MathElement::GetRefObjectName(const Gmat::ObjectType type) const
{
   switch (type)
   {
      case Gmat::MATH_NODE:
         return refObjectName;
         
      default:
         break;
   }
   
   return GmatBase::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets the name of the referenced objects.
 *
 * @return true when referenced object name is set sucessfully otherwise false.
 */
 //------------------------------------------------------------------------------
bool MathElement::SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
{ 
   switch (type)
   {
      case Gmat::MATH_NODE:
         refObjectName = name;
         return true;
         
      default:
         break;
   }
   
   return GmatBase::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
Real MathElement::Evaluate() const
{
	 // @todo: This section may need more implementation.
    return MathNode::Evaluate();
}

//------------------------------------------------------------------------------
// bool EvaluateInputs()
//------------------------------------------------------------------------------
/**
 * This method always returns true if the node was sucessfully initialized.
 */
 //------------------------------------------------------------------------------
bool MathElement::EvaluateInputs() const
{
	if (refObject)
      return true;
   else
      return MathNode::EvaluateInputs();
}

//------------------------------------------------------------------------------
// void ReportOutputs()
//------------------------------------------------------------------------------
void MathElement::ReportOutputs(Integer &type, 
                                Integer &rowCount, Integer &colCount) const
{
}

//------------------------------------------------------------------------------
// bool MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix MathElement::MatrixEvaluate()
{
	return MathNode::MatrixEvaluate();
}

