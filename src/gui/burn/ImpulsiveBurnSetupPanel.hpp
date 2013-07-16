//$Id$
//------------------------------------------------------------------------------
//                              ImpulsiveBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: LaMont Ruley
// Created: 2004/02/04
//
/**
 * This class contains the Impulsive Burn Setup window.
 */
//------------------------------------------------------------------------------
#ifndef ImpulsiveBurnSetupPanel_hpp
#define ImpulsiveBurnSetupPanel_hpp

#include "BurnThrusterPanel.hpp"

class ImpulsiveBurnSetupPanel : public BurnThrusterPanel
{
public:
   // constructors
   ImpulsiveBurnSetupPanel(wxWindow *parent, const wxString &name);
   ~ImpulsiveBurnSetupPanel();
    
protected:
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData(GmatBase *theObject);
   
private:
};
#endif // ImpulsiveBurnSetupPanel_hpp
