//$Header$
//------------------------------------------------------------------------------
//                              TableTemplate
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: M. Weippert, T. McRoberts, E. Corderman
// Created: 1995/10/10 for GSS project
// Modified: 2003/09/15 Linda Jun - Replaced GSSString with std::string.
//
/**
 * Provies declarations for the TableTemplate template class, representing
 * a 2-dimensional table of any type T (see assumptions about type T, below)
 */
//------------------------------------------------------------------------------
#ifndef TableTemplate_hpp
#define TableTemplate_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp"

//  exceptions
class TableTemplateExceptions
{
public:

    class DimensionError : public BaseException 
          {public : DimensionError(const std::string &message =
          "TableTemplate error : dimension error or mismatch.")
          : BaseException(message) {};  };
    class OutOfBounds : public BaseException
          {public : OutOfBounds(const std::string &message =
          "TableTemplate error : index out-of-bounds.")
          : BaseException(message) {};  };
    class UnsizedTable : public BaseException 
          {public : UnsizedTable(const std::string &message =
          "TableTemplate error : unsized table.")
          : BaseException(message) {};  };
    class TableAlreadySized : public BaseException 
          {public : TableAlreadySized(const std::string &message =
          "TableTemplate error : table already sized.")
          : BaseException(message) {};  };
    class IllegalSize : public BaseException 
          {public : IllegalSize(const std::string &message =
          "TableTemplate error : illegal (negative) size.")
          : BaseException(message) {};  };

private:
};

template <class T>
class TableTemplate
{

/**
 *  Tables are stored in row major order.
 *  Assumptions about template parameter types:
 *      type has appropriate initializers (constructors and "=" operator)
 *  The exceptions are declared in a separate class because the current HP
 *  compiler cannot properly handle exceptions declared a template class 
 *  and thrown in another template class.
 */

public:
    TableTemplate();
    TableTemplate(Integer r, Integer c);
    // HP won't compile this 
    // TableTemplate(Integer r, Integer c, const T &a11,...);
    TableTemplate(Integer r, Integer c, const T* array);
    TableTemplate(const TableTemplate<T> &table);
    virtual ~TableTemplate();

    T& operator()(Integer r, Integer c);
    const T& operator()(Integer r, Integer c) const;
    TableTemplate<T>& operator=(const TableTemplate<T> &table);
    bool operator==(const TableTemplate<T> &table) const;
    bool operator!=(const TableTemplate<T> &table) const;
    
    virtual T    GetElement(Integer r, Integer c) const;
    virtual void SetElement(Integer r, Integer c, const T &value);
    virtual bool IsSized() const;
    virtual void SetSize(Integer r, Integer c);
    virtual void GetSize(Integer &r, Integer &c) const;
    virtual Integer  GetNumColumns() const;
    virtual Integer  GetNumRows() const;
    
protected:
    T   *elementD;
    Integer rowsD, colsD;
    bool isSizedD;
    void init(Integer r, Integer c);

private:
};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "TableTemplate.cpp"
#endif

#endif // TableTemplate_hpp
