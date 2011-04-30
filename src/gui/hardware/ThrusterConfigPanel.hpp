//$Id$
//------------------------------------------------------------------------------
//                            ThrusterConfigPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Waka Waktola
// Created: 2005/01/06
/**
 * This class contains information needed to setup users spacecraft thruster 
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef ThrusterConfigPanel_hpp
#define ThrusterConfigPanel_hpp

#include "BurnThrusterPanel.hpp"

class ThrusterConfigPanel: public BurnThrusterPanel
{
public:
   ThrusterConfigPanel(wxWindow *parent, const wxString &name);
   ~ThrusterConfigPanel();
   
protected:
   
   virtual void LoadData();
   virtual void SaveData();
   
private:
};
#endif

