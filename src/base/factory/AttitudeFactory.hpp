//$Header$
//------------------------------------------------------------------------------
//                         AttitudeFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Wendy Shoan
// Created: 2006.03.16
//
/**
 *  This class is the factory class for Attitude objects.  
 */
//------------------------------------------------------------------------------
#ifndef AttitudeFactory_hpp
#define AttitudeFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Attitude.hpp"

class GMAT_API AttitudeFactory : public Factory
{
public:
   Attitude*  CreateAttitude(const std::string &ofType,
                             const std::string &withName = "");

   // default constructor
   AttitudeFactory();
   // constructor
   AttitudeFactory(StringArray createList);
   // copy constructor
   AttitudeFactory(const AttitudeFactory& fact);
   // assignment operator
   AttitudeFactory& operator= (const AttitudeFactory& fact);

   // destructor
   ~AttitudeFactory();

protected:
   // protected data

private:
   // private data


};

#endif // AttitudeFactory_hpp




