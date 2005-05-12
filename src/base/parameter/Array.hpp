//$Header$
//------------------------------------------------------------------------------
//                                  Array
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/20
//
/**
 * Declares Array class which handles one or two dimentional array.
 */
//------------------------------------------------------------------------------
#ifndef Array_hpp
#define Array_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"

class GMAT_API Array : public Parameter
{
public:

   Array(const std::string &name = "", const std::string &desc = "",
         const std::string &unit = "");
   Array(const Array &copy);
   Array& operator= (const Array& right);
   virtual ~Array();

   bool operator==(const Array &right) const;
   bool operator!=(const Array &right) const;

   bool SetSize(const Integer row, const Integer col);

   //-----------------------------------------------------------------
   //loj: 9/20/04 Do we need to add thise methods to GmatBase?
   //-----------------------------------------------------------------
   virtual Real GetRealParameter(const Integer id, const Integer row,
                                 const Integer col) const;
   virtual Real GetRealParameter(const std::string &label, const Integer row,
                                 const Integer col) const;
   virtual Real SetRealParameter(const Integer id, const Real value,
                                 const Integer row, const Integer col);
   virtual Real SetRealParameter(const std::string &label,
                                 const Real value, const Integer row,
                                 const Integer col);
   virtual const std::string& GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName);

   virtual Rvector GetRvectorParameter(const Integer id,
                                       const Integer index) const;
   virtual const Rvector& SetRvectorParameter(const Integer id,
                                              const Rvector &value,
                                              const Integer index);
   virtual Rvector GetRvectorParameter(const std::string &label,
                                       const Integer index) const;
   virtual const Rvector& SetRvectorParameter(const std::string &label,
                                              const Rvector &value,
                                              const Integer index);
   
   //-----------------------------------------------------------------
  
   // methods inherited from Parameter
   virtual std::string ToString();
   virtual const Rmatrix& GetMatrix() const;
   virtual const std::string* GetParameterList() const;
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool IsParameterReadOnly(const Integer id) const;

   virtual Integer GetIntegerParameter(const Integer id) const;
   virtual Integer GetIntegerParameter(const std::string &label) const;
   virtual Integer SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer SetIntegerParameter(const std::string &label,
                                       const Integer value);
  
   virtual const Rmatrix& GetRmatrixParameter(const Integer id) const;
   virtual const Rmatrix& GetRmatrixParameter(const std::string &label) const;
   virtual const Rmatrix& SetRmatrixParameter(const Integer id,
                                              const Rmatrix &value);
   virtual const Rmatrix& SetRmatrixParameter(const std::string &label,
                                              const Rmatrix &value);
   
protected:

   Integer mNumRows;
   Integer mNumCols;
   bool mSizeSet;
   Rmatrix mRmatValue;
   
   enum
   {
      NUM_ROWS = ParameterParamCount,
      NUM_COLS,
      RMAT_VALUE,
      SINGLE_VALUE,
      ROW_VALUE,
      COL_VALUE,
      ArrayParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[ArrayParamCount - ParameterParamCount];
   static const std::string
      PARAMETER_TEXT[ArrayParamCount - ParameterParamCount];
    
private:

};
#endif // Array_hpp
