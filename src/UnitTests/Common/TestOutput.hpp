//$Header: /cygdrive/p/dev/cvs/test/common/TestOutput.hpp,v 1.15 2008/08/22 14:46:37 lojun Exp $
//------------------------------------------------------------------------------
//                             TestOutput
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/02/11
//
/**
 * Declares TestOutput class which provides output methods to console and file.
 */
//------------------------------------------------------------------------------
#ifndef TestOutput_hpp
#define TestOutput_hpp

#include "gmatdefs.hpp"
#include <fstream>

#if !defined __TEST_NON_REAL__
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#endif

class TestOutput
{
public:

   TestOutput(const std::string &filename);
   
   std::string GetOutPathName();
   std::string GetOutFileName();
   
   void Close();
   void SetWidth(int w);
   void SetAddNewLine(bool addNewLine);
   
   void PutBool(bool bval);
   void Put(int ival);
   
   void Put(const std::string &str);
   void PutLine(const std::string &str);
   void Put(const std::string &str1, int ival1,
            const std::string &str2 = "", int ival2 = -99999,
            const std::string &str3 = "", int ival3 = -99999);
   void Put(const std::string &str, bool bval);
   void Put(const std::string &str, const char *sval);
   void Put(const std::string &str, const std::string &sval);
   void Put(const std::string &str1, const std::string &str2,
            const std::string &str3);
   void Put(int ival1, const std::string &str1);
   
   void Validate(bool actual, bool expect, bool validate = true);
   void Validate(const std::string &actual, const std::string &expect,
                 bool validate = true);
   void Validate(const std::string &actual1, const std::string &actual2,
                 const std::string &expect1, const std::string &expect2,
                 bool validate = true);
   
   #if !defined __TEST_NON_REAL__
   void Validate(int actual, int expect, bool validate = true);
   void Validate(int actual1, int actual2, int expect1, int expect2,
                 bool validate = true);
   
   //static const Real TEST_TOL = 1.0e-5;
   static const Real TEST_TOL;
   
   void SetPrecision(int p);
   
   void Put(Real rval);
   void Put(const std::string &str, Real rval1, Real rval2, Real rval3);
   void Put(const std::string &str1, Real rvalx, const std::string &str2,
            Real rval1, Real rval2, Real rval3);
   void Put(Real rval1, const std::string &str, Real rval2);
   void Put(Real rval1, const std::string &str1,
            Real rval2, const std::string &str2);
   void Put(Real rval1, const std::string &str1,
            Real rval2, const std::string &str2, Real rval3);
   
   void Put(const std::string &str, Real rval);
   void Put(const std::string &str1, Real rval1,
            const std::string &str2, Real rval2);
   void Put(const std::string &str1, Real rval, const std::string &str2);
   
   void Put(const Rmatrix &rmat);
   void Put(const Rvector &rvec);
   void Put(const std::string &str, const Rmatrix &rmat);
   void Put(const std::string &str, const Rvector &rvec);
   void CheckValue(Real actual, Real expect, Real tol = TEST_TOL);
   void Validate(Real actual, Real expect, Real tol = TEST_TOL,
                 bool validate = true);
   void Validate(Real actual1, Real actual2, Real actual3,
                 Real expect1, Real expect2, Real expect3, Real tol = TEST_TOL,
                 bool validate = true);
   void Validate(Rmatrix actual, Rmatrix expect, Real tol = TEST_TOL,
                 bool validate = true);
   void Validate(Rvector actual, Rvector expect, Real tol = TEST_TOL,
                 bool validate = true);
   #endif
   
private:
   
   std::string mOutFileName;
   std::string mOutPathName;
   std::ofstream mOutfile;
   
   int mWidth;
   int mPrecision;
   bool mAddNewLine;
};

#endif
