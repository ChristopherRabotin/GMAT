//$Id$
//------------------------------------------------------------------------------
//                                 UndockedMissionPanel
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
// Created: 2011/08/25
//
/**
 * Shows undocked Mission panel
 */
//------------------------------------------------------------------------------
#ifndef UndockedMissionPanel_hpp
#define UndockedMissionPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "MissionTreeToolBar.hpp"

class MissionTree;
class GmatNotebook;

class UndockedMissionPanel: public GmatPanel
{
public:
   
   UndockedMissionPanel(wxWindow *parent, const wxString &name);
   ~UndockedMissionPanel();
   
   MissionTree*         GetMissionTree();
   MissionTreeToolBar*  GetMissionToolBar();
   void                 SetGmatNotebook(GmatNotebook *notebook);
   void                 ResetMissionTreeTools();
   void                 SetMissionTreeExpandLevel(int level);
   
private:
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   wxWindow             *mParent;
   GmatNotebook         *mGmatNotebook;
   MissionTree          *mMissionTree;
   MissionTreeToolBar   *mMissionTreeToolBar;
   
   DECLARE_EVENT_TABLE();
   
   void                 OnClose(wxCloseEvent& event);
   
};

#endif
