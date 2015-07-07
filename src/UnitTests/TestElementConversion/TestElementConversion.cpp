//$Header: /cygdrive/p/dev/cvs/test/TestUtil/TestElementConversion.cpp,v 1.2 2007/02/01 21:33:29 lojun Exp $
//------------------------------------------------------------------------------
//                              TestElementConversion
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2005/02/14
//
/**
 * Test driver for converting between different element types.
 * (Cartesian, Keplerian, SphericalRADEC, SphericalAZFPA)
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "Rvector6.hpp"
#include "CoordUtil.hpp"
#include "SphericalRADEC.hpp"
#include "TestOutput.hpp"

using namespace std;


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{

   out.Put("\n============================== test CoordUtil");
   
   Rvector6 cartState(7100.0, 0.0, 1300.0, 0.0, 7.35, 1.0);
   Real grav = 398600.4415;
   Real ma;
   out.Put("========================= CartesianToKeplerian()");
   out.Put("cartState = ", cartState.ToString());
   
   Rvector6 keplState = CartesianToKeplerian(cartState, grav, &ma);
   out.Put("keplState = ", keplState.ToString());
   
   cout << "=============== Convert back to Cartesian()" << endl;
   Rvector6 cartState1 = KeplerianToCartesian(keplState, grav, CoordUtil::TA);
   out.Put("cartState1 = ", cartState1.ToString());
   
   out.Put("=============== Change ecc to 1.5 and convert to Cartesian()");
   keplState[1] = 1.5;
   cartState1 = KeplerianToCartesian(keplState, grav, CoordUtil::TA);
   out.Put("cartState1 = ", cartState1.ToString());
   
   out.Put("========================= CartesianToSphericalRADEC()");
   out.Put("cartState = ", cartState.ToString());
   
   Rvector6 radecState = CartesianToSphericalRADEC(cartState);
   out.Put("radecState = ", radecState.ToString());
   
   out.Put("=============== Convert back to Cartesian()");
   Rvector6 radecState1 = SphericalRADECToCartesian(radecState);
   out.Put("cartState1 = ", radecState1.ToString());

}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\Test\\TestUtil\\TestElementConversion.out");
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of element conversion!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
