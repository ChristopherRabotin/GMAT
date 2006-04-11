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
             MathNode      ("MathElement", nomme),
             refObject     (NULL),
             refObjectName (""),
             elementType   (Gmat::REAL_TYPE)
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
             refObjectName (me.refObjectName),
             elementType   (me.elementType)
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
      case Gmat::PARAMETER:
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
      case Gmat::PARAMETER:
         refObject = (Parameter*) obj;
         refObjectName = name;
         elementType = Gmat::RMATRIX_TYPE;
         
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
      case Gmat::PARAMETER:
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
      case Gmat::PARAMETER:
         refObjectName = name;
         elementType = Gmat::RMATRIX_TYPE;
         return true;
         
      default:
         break;
   }
   
   return GmatBase::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object.
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& MathElement::GetRefObjectNameArray(const Gmat::ObjectType type)
{
	static StringArray refs;
	
   if (type == Gmat::PARAMETER)
   {
      refs.push_back(refObjectName);
      return refs;
   }
   else if (type == Gmat::UNKNOWN_OBJECT)
   {
   	   refs.clear();
   	   return refs;
   }
   
   return GmatBase::GetRefObjectNameArray(type);
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
Real MathElement::Evaluate()
{
   if (refObject)
   {
   	   realValue = refObject->EvaluateReal();
   	   return realValue;
   }
   else if (realValue != 0.0)
      return realValue;
   
   return MathNode::Evaluate();
}

//------------------------------------------------------------------------------
// bool EvaluateInputs()
//------------------------------------------------------------------------------
/**
 * This method always returns true if the node was sucessfully initialized.
 */
 //------------------------------------------------------------------------------
bool MathElement::EvaluateInputs()
{
	if (refObject)
      return true;
   
   return MathNode::EvaluateInputs();
}

//------------------------------------------------------------------------------
// void ReportOutputs()
//------------------------------------------------------------------------------
void MathElement::ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount)
{
	type = elementType;
	
	if (type == Gmat::RMATRIX_TYPE)
	{
	   rowCount = matrix.GetNumRows();
	   colCount = matrix.GetNumColumns();
	}
	else if (type == Gmat::REAL_TYPE)
	{
		rowCount = 1;
	   colCount = 1;
	}
}

//------------------------------------------------------------------------------
// bool MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix MathElement::MatrixEvaluate()
{
   if (elementType == Gmat::RMATRIX_TYPE)
      return matrix;
	   
	return MathNode::MatrixEvaluate();
}

//------------------------------------------------------------------------------
// Real SetRealValue(Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of the node when it contains a constant.
 */
 //------------------------------------------------------------------------------
Real MathElement::SetRealValue(Real value)
{	
	realValue = value;
	elementType = Gmat::REAL_TYPE;
	return realValue;
}

