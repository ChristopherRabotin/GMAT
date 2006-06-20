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
#include "StringUtil.hpp"  // for ToDouble()
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_MATH_ELEMENT 1


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
   refObjectName ("")
{
   isFunction = false;
   
   Real rval;
   if (GmatStringUtil::ToDouble(nomme, &rval))
   {
      SetRealValue(rval);
      SetNumberFlag(true);
   }
   else
   {
      SetRefObjectName(Gmat::PARAMETER, nomme);
   }

   #if DEBUG_MATH_ELEMENT
   MessageInterface::ShowMessage
      ("MathElement() type=%s, name=%s created\n", typeStr.c_str(), nomme.c_str());
   #endif
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
             refObjectType (me.refObjectType)
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
        
   MathNode::operator=(me);
   refObject = me.refObject;
   refObjectName = me.refObjectName;
   refObjectType = me.refObjectType;

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
      {
         // Handle array index
         Integer row, col;
         std::string newName;
         GmatStringUtil::GetArrayIndex(refObjectName, row, col, newName);
         
         // Check if name is the same
         //if (refObjectName != name)
         if (newName != name)
            throw MathException
               ("MathElement::SetRefObject() Cannot find parameter name:" +
                name + "\n");
         
         refObject = (Parameter*) obj;
         
         refObjectType = refObject->GetTypeName().c_str();
         
         //if (refObject->GetReturnType() == Gmat::RMATRIX_TYPE)
         
         if (refObjectType == "Array")
         {
            Array *arr = (Array*)refObject;     
            elementType = Gmat::RMATRIX_TYPE;
            Integer theRowCount = arr->GetRowCount();
            Integer theColCount = arr->GetColCount();
            
            #if DEBUG_MATH_ELEMENT
            MessageInterface::ShowMessage
               ("MathElement::SetRefObject() elementType=%d, theRowCount=%d, "
                "theColCount=%d\n", elementType, theRowCount, theColCount);
            #endif
            
            if (!matrix.IsSized())
               matrix.SetSize(theRowCount, theColCount);
            else
            {
               #if DEBUG_MATH_ELEMENT
               MessageInterface::ShowMessage
                  ("MathElement::SetRefObject() matrix already sized. "
                   "matrix.size=%d, %d\n", matrix.GetNumRows(),
                   matrix.GetNumColumns());
               #endif
            }
            
            matrix = arr->GetRmatrix(); // initial value
            
            #if DEBUG_MATH_ELEMENT
            std::stringstream ss("");
            ss << matrix;
            MessageInterface::ShowMessage
               ("MathElement::SetRefObject() name=%s, matrix=\n%s\n", name.c_str(),
                ss.str().c_str());
            #endif
            
         }
         else if (refObject->GetReturnType() == Gmat::REAL_TYPE)
         {
            elementType = Gmat::REAL_TYPE;
            realValue = refObject->GetReal(); // initial value
            
            #if DEBUG_MATH_ELEMENT
            MessageInterface::ShowMessage
               ("MathElement::SetRefObject() name=%s, elementType=%d, "
                "realValue=%f\n", GetName().c_str(), elementType, realValue);
            #endif
         }
         
         return true;
      }
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
   #if DEBUG_MATH_ELEMENT
   MessageInterface::ShowMessage
      ("==> MathElement::SetRefObjectName() name=%s\n", name.c_str());
   #endif
   
   switch (type)
   {
      case Gmat::PARAMETER:
         refObjectName = name;
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
   refs.clear();
   
   if (type == Gmat::PARAMETER || Gmat::UNKNOWN_OBJECT)
   {
      Integer row, col;
      std::string newName;
      GmatStringUtil::GetArrayIndex(refObjectName, row, col, newName);
      
      refs.push_back(newName);
      return refs;
   }
   
   return GmatBase::GetRefObjectNameArray(type);
}


// Inherited MathNode methods
//------------------------------------------------------------------------------
// virtual void SetMatrixValue(const Rmatrix &mat)
//------------------------------------------------------------------------------
void MathElement::SetMatrixValue(const Rmatrix &mat)
{
//	if (refObject)
//	{
//	   refObjectType = refObject->GetTypeName().c_str();
//	   
//	   if (refObjectType == "Array")
//	      return MathNode::Evaluate();
//	   else
//	   {
//	      realValue = refObject->EvaluateReal();
//   	      return realValue;
//	   }
//	}
//   else if (realValue != 0.0)
//      return realValue;
//   
//   return MathNode::Evaluate();

   // if we can set matrix value, it is not a parameter
   isNumber = true;
   refObjectName = "";

   MathNode::SetMatrixValue(mat);
}


//------------------------------------------------------------------------------
// void GetOutputInfo()
//------------------------------------------------------------------------------
void MathElement::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = elementType;
   rowCount = 1;
   colCount = 1;
   
   #if DEBUG_MATH_ELEMENT
   MessageInterface::ShowMessage
      ("MathElement::GetOutputInfo() isNumber=%d, refObjectName=%s\n",
       isNumber, refObjectName.c_str());
   #endif
   
   if (refObjectName == "")
   {
      if (type == Gmat::RMATRIX_TYPE)
      {
         rowCount = matrix.GetNumRows();
         colCount = matrix.GetNumColumns();
      }
   }
   else
   {
      #if DEBUG_MATH_ELEMENT
      MessageInterface::ShowMessage
         ("MathElement::GetOutputInfo() %s is parameter. refObjectName=%s\n",
          GetName().c_str(), refObjectName.c_str());
      #endif
      
      if (refObject)
      {
         type = refObject->GetReturnType();
         
         if (type == Gmat::RMATRIX_TYPE)
         {
            // Handle array index
            Integer row, col;
            std::string newName;
            GmatStringUtil::GetArrayIndex(refObjectName, row, col, newName);

            if (row != -1 && col != -1)
            {
               type = Gmat::REAL_TYPE;
               rowCount = 1;
               colCount = 1;
            }
            else
            {
               rowCount = ((Array*)refObject)->GetRowCount();
               colCount = ((Array*)refObject)->GetColCount();
            }
         }
         
         #if DEBUG_MATH_ELEMENT
         MessageInterface::ShowMessage
            ("MathElement::GetOutputInfo() type=%d, row=%d, col=%d\n", type,
             rowCount, colCount);
         #endif
      }
      else
      {
         throw MathException("The parameter: " + GetName() + " is NULL\n");
      }
   }
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method always returns true if the node was sucessfully initialized.
 */
 //------------------------------------------------------------------------------
bool MathElement::ValidateInputs()
{
   if (elementType == Gmat::REAL_TYPE && refObjectName == "")
      return true;

   if (refObject)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
Real MathElement::Evaluate()
{
   if (refObject)
   {
      if (elementType == Gmat::REAL_TYPE)
      {
         realValue = refObject->EvaluateReal();
      }
      else if (elementType == Gmat::RMATRIX_TYPE)
      {
         // Handle array index
         Integer row, col;
         std::string newName;
         GmatStringUtil::GetArrayIndex(refObjectName, row, col, newName);
         
         if (row != -1 && col!= -1)
         {
            realValue = refObject->EvaluateRmatrix().GetElement(row, col);
         }
         else
         {
            std::stringstream ss("");
            ss << "MathElement::Evaluate() Array has invalid index. row=" << row
               << " col=" << col << "\n";
            
            throw MathException(ss.str());
         }
      }
      
      #if DEBUG_MATH_ELEMENT
      MessageInterface::ShowMessage
         ("MathElement::Evaluate() It's a parameter: %s realValue = %f\n",
          refObject->GetName().c_str(), realValue);
      #endif
      
      return realValue;
   }
   else
   {
      #if DEBUG_MATH_ELEMENT
      MessageInterface::ShowMessage
         ("MathElement::Evaluate() It's a number: realValue = %f\n", realValue);
      #endif
      
      return realValue;
   }
   
   return MathNode::Evaluate();
}


//------------------------------------------------------------------------------
// bool MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix MathElement::MatrixEvaluate()
{
   if (elementType == Gmat::RMATRIX_TYPE)
   {
      if (refObject)
      {
         #if DEBUG_MATH_ELEMENT
         Rmatrix rmat = refObject->GetRmatrix();
         MessageInterface::ShowMessage
            ("MathElement::Evaluate() It's an Array: %s matVal =\n%s\n",
             refObject->GetName().c_str(), rmat.ToString().c_str());
         #endif
         
         return refObject->GetRmatrix();
      }
      else
      {
         #if DEBUG_MATH_ELEMENT
         MessageInterface::ShowMessage
            ("MathElement::Evaluate() It's a Rmatrix. matVal =\n%s\n",
             matrix.ToString().c_str());
         #endif
         
         return matrix;
      }
   }
   else
      throw MathException("MathElement::MatrixEvaluate() Invalid matrix\n");
}


//------------------------------------------------------------------------------
// bool SetChildren(MathNode *leftChild, MathNode *rightChild)
//------------------------------------------------------------------------------
bool MathElement::SetChildren(MathNode *leftChild, MathNode *rightChild)
{
   throw MathException("SetChildren() is not valid for MathElement\n");
}


//------------------------------------------------------------------------------
// MathNode* GetLeft()
//------------------------------------------------------------------------------
MathNode* MathElement::GetLeft()
{
   return NULL;
}


//------------------------------------------------------------------------------
// MathNode* GetRight()
//------------------------------------------------------------------------------
MathNode* MathElement::GetRight()
{
   return NULL;
}
