//$Id$
//------------------------------------------------------------------------------
//                                 PenUp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/02/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Class implementation for the PenUp command
 */
//------------------------------------------------------------------------------


#ifndef PenUp_hpp
#define PenUp_hpp

#include "GmatCommand.hpp"
//#include "XyPlot.hpp"
#include "Subscriber.hpp"


/**
 * Command used to stop drawing data on an XY plot during a run.
 */
class GMAT_API PenUp : public GmatCommand
{
public:
        PenUp();
        virtual          ~PenUp();
   PenUp(const PenUp &c);
   PenUp&        operator=(const PenUp &c);
   
   virtual GmatBase* Clone() const;

   virtual const ObjectTypeArray&
                     GetRefObjectTypeArray();
   virtual const StringArray&
                     GetRefObjectNameArray(const Gmat::ObjectType type);

   bool              InterpretAction();
   bool              Initialize();
   bool              Execute();

protected:
   //std::string plotName;
   //XyPlot      *thePlot;
   StringArray          plotNameList;   
   //std::vector<XyPlot*> thePlotList;
   std::vector<Subscriber*> thePlotList;
};

#endif /* PenUp_hpp */
