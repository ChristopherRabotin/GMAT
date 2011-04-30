//$Id$
//------------------------------------------------------------------------------
//                              GmatCommandPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2010/07/15
/**
 * Declares GmatCommandPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef GmatCommandPanel_hpp
#define GmatCommandPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"

class GmatCommandPanel: public GmatPanel
{
public:
   GmatCommandPanel(wxWindow *parent, GmatCommand *cmd);
   ~GmatCommandPanel();
      
protected:

   GmatCommand *theCommand;
   wxTextCtrl *commandTextCtrl;
   
   void OnTextChange(wxCommandEvent &event);   
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT_CTRL = 93000,
   };

private:
};
#endif
