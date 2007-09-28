//$Header$
//------------------------------------------------------------------------------
//                               ClearPlot
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2004/02/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Class implementation for the ClearPlot command
 */
//------------------------------------------------------------------------------


#ifndef ClearPlot_hpp
#define ClearPlot_hpp

#include "GmatCommand.hpp"
#include "TsPlot.hpp"


/**
 * Command used to remove data from an XY plot during a run
 */
class ClearPlot : public GmatCommand
{
public:
	ClearPlot();
	virtual          ~ClearPlot();
   ClearPlot(const ClearPlot &c);
   ClearPlot&        operator=(const ClearPlot &c);
   
   virtual GmatBase* Clone() const;
   
   virtual const ObjectTypeArray&
                     GetRefObjectTypeArray();
   virtual const StringArray&
                     GetRefObjectNameArray(const Gmat::ObjectType type);

   bool              InterpretAction();
   bool              Initialize();
   bool              Execute();

protected:
   StringArray          plotNameList;   
   std::vector<TsPlot*> thePlotList;
};

#endif /* ClearPlot_hpp */
