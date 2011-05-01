//$Id: MatlabWorkspaceFactory.hpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                            MatlabWorkspaceFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Linda Jun
// Created: 2010/03/30
//
/**
 *  Declaration code for the MatlabWorkspaceFactory class. With this class user can
 *  write data to MATLAB workspace as data is published from the Publisher.
 */
//------------------------------------------------------------------------------
#ifndef MatlabWorkspaceFactory_hpp
#define MatlabWorkspaceFactory_hpp


#include "matlabinterface_defs.hpp"
#include "Factory.hpp"
#include "Subscriber.hpp"

class MATLAB_API MatlabWorkspaceFactory : public Factory
{
public:
   virtual Subscriber* CreateSubscriber(const std::string &ofType,
                                        const std::string &withName = "",
                                        const std::string &fileName = "");
   
   // default constructor
   MatlabWorkspaceFactory();
   // constructor
   MatlabWorkspaceFactory(StringArray createList);
   // copy constructor
   MatlabWorkspaceFactory(const MatlabWorkspaceFactory& fact);
   // assignment operator
   MatlabWorkspaceFactory& operator=(const MatlabWorkspaceFactory& fact);
   
   virtual ~MatlabWorkspaceFactory();
   
};

#endif // MatlabWorkspaceFactory_hpp
