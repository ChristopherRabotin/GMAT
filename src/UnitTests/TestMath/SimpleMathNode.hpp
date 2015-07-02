//$Header: /cygdrive/p/dev/cvs/test/TestMath/SimpleMathNode.hpp,v 1.2 2006/05/02 16:47:39 lojun Exp $
//------------------------------------------------------------------------------
//                                   SimpleMathNode
//------------------------------------------------------------------------------
// This class was created to unit test MathParser.
//------------------------------------------------------------------------------

#ifndef SimpleMathNode_hpp
#define SimpleMathNode_hpp

#include "gmatdefs.hpp"
#include "Rmatrix.hpp"
#include "Parameter.hpp"

/**
 * All function classes are derived from this base class.
 */
class GMAT_API SimpleMathNode
{
public:
   SimpleMathNode(const std::string &typeStr, const std::string &name = "");
   virtual ~SimpleMathNode();

   std::string GetTypeName() { return theType; }
   std::string GetName() { return theName; }

   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();
   
   virtual bool ValidateInputs();
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   
   //*** This should be in MathFunction
   void SetChildren(SimpleMathNode *leftNode, SimpleMathNode *rightNode);
   SimpleMathNode* GetLeft() { return theLeft; }
   SimpleMathNode* GetRight() { return theRight; }
   
   //*** This should be in MathElement
   void SetRealValue(Real val);
   void SetMatrixValue(const Rmatrix &mat);

//    virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                   const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");
//    virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);
   
protected:
   
   std::string theType;
   std::string theName;
   SimpleMathNode *theLeft;
   SimpleMathNode *theRight;
   
   Real realValue;
   Rmatrix rmat;

   //*** Do we want to add this to MathNode?
   Parameter *theParameter;
   bool isParameter;
   Integer theReturnType;
   Integer theRowCount;
   Integer theColCount;
   Integer theArgCount;
};


#endif //SimpleMathNode_hpp


